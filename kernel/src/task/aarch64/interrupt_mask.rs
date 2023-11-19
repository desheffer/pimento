use core::arch::asm;
use core::cell::UnsafeCell;

/// A simple wrapper for enabling and disabling interrupts
#[derive(Debug)]
pub struct InterruptMask {
    level: UnsafeCell<i64>,
    saved_daif: UnsafeCell<u64>,
}

impl InterruptMask {
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

    unsafe fn _enable_interrupts() {
        asm!("msr daifclr, #0b0011");
    }

    unsafe fn _disable_interrupts() {
        asm!("msr daifset, #0b0011");
    }

    unsafe fn _save() -> u64 {
        let daif: u64;
        asm!("mrs {}, daif", out(reg) daif);
        daif
    }

    unsafe fn _restore(daif: u64) {
        asm!("msr daif, {}", in(reg) daif);
    }

    pub fn enable_interrupts(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            Self::_disable_interrupts();
            assert!(*self.level.get() == 0);

            Self::_enable_interrupts();
        }
    }

    pub fn disable_interrupts(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            Self::_disable_interrupts();
            assert!(*self.level.get() == 0);
        }
    }

    pub fn lock(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            let saved = Self::_save();
            Self::_disable_interrupts();

            if *self.level.get() == 0 {
                *self.saved_daif.get() = saved;
            }

            *self.level.get() += 1;
        }
    }

    pub fn unlock(&self) {
        // SAFETY: Safe because interrupts are disabled.
        unsafe {
            Self::_disable_interrupts();

            assert!(*self.level.get() > 0);
            *self.level.get() -= 1;

            if *self.level.get() == 0 {
                Self::_restore(*self.saved_daif.get());
            }
        }
    }

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
