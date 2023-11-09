use core::arch::asm;
use core::cell::UnsafeCell;

/// A simple lock
///
/// This is a poor implementation of a lock that's not actually atomic (when interrupts are
/// enabled). Despite such a flaw, this lock works "well enough" and serves as a foundation for
/// when more robust implementations are possible.
#[derive(Debug)]
pub struct Lock {
    locked: UnsafeCell<bool>,
}

impl Lock {
    pub const fn new() -> Self {
        Self {
            locked: UnsafeCell::new(false),
        }
    }

    pub fn lock(&self) {
        // TODO: Use the core::intrinsics::coreatomic_cxchg_* functions once the MMU is enabled.
        // SAFETY: Not actually safe!
        unsafe {
            // Spin until the lock is not held.
            while *self.locked.get() {
                asm!("wfi");
            }

            *self.locked.get() = true;
        }
    }

    pub fn unlock(&self) {
        // TODO: Use the core::intrinsics::coreatomic_cxchg_* functions once the MMU is enabled.
        // SAFETY: Not actually safe!
        unsafe {
            *self.locked.get() = false;
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

unsafe impl Sync for Lock {}
