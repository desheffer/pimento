use core::arch::asm;
use core::cell::UnsafeCell;

/// A simple wrapper for enabling and disabling interrupts.
pub struct InterruptMask {
    level: UnsafeCell<i64>,
    saved_daif: UnsafeCell<u64>,
}

impl InterruptMask {
    /// Gets the interrupt mask.
    pub fn instance() -> &'static Self {
        static INSTANCE: InterruptMask = InterruptMask::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            level: UnsafeCell::new(0),
            saved_daif: UnsafeCell::new(0),
        }
    }

    /// Enables interrupts.
    pub fn enable_interrupts(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            disable_interrupts();
            assert!(*self.level.get() == 0);

            enable_interrupts();
        }
    }

    /// Disables interrupts.
    pub fn disable_interrupts(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            disable_interrupts();
            assert!(*self.level.get() == 0);
        }
    }

    /// Disables interrupts and obtains the lock. If the lock was not previously held, then the
    /// original interrupt state is saved.
    pub fn lock(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            let saved = save();
            disable_interrupts();

            if *self.level.get() == 0 {
                *self.saved_daif.get() = saved;
            }

            *self.level.get() += 1;
        }
    }

    /// Releases the lock. If the lock is no longer held, then the original interrupt state is
    /// restored.
    pub fn unlock(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            disable_interrupts();

            assert!(*self.level.get() > 0);
            *self.level.get() -= 1;

            if *self.level.get() == 0 {
                restore(*self.saved_daif.get());
            }
        }
    }

    /// Executes the given closure while holding the lock.
    pub fn call<F, R>(&self, f: F) -> R
    where
        F: FnOnce() -> R,
    {
        self.lock();

        let result = f();

        self.unlock();
        result
    }
}

unsafe impl Send for InterruptMask {}
unsafe impl Sync for InterruptMask {}

unsafe fn enable_interrupts() {
    asm!("msr daifclr, #0b0011");
}

unsafe fn disable_interrupts() {
    asm!("msr daifset, #0b0011");
}

unsafe fn save() -> u64 {
    let daif: u64;
    asm!("mrs {}, daif", out(reg) daif);
    daif
}

unsafe fn restore(daif: u64) {
    asm!("msr daif, {}", in(reg) daif);
}
