use core::arch::asm;
use core::cell::UnsafeCell;

use crate::cpu::InterruptMask;

/// A simple spin lock.
///
/// This lock assumes that the system has a single core.
pub struct Lock {
    locked: UnsafeCell<bool>,
}

impl Lock {
    /// Creates a lock.
    pub const fn new() -> Self {
        Self {
            locked: UnsafeCell::new(false),
        }
    }

    /// Obtains the lock.
    pub fn lock(&self) {
        // TODO: Use the core::intrinsics::coreatomic_cxchg_* functions once the MMU is enabled.
        // SAFETY: Safe on a single core because interrupts are disabled.
        unsafe {
            // Spin until the lock is acquired.
            loop {
                let acquired = InterruptMask::instance().call(|| match *self.locked.get() {
                    true => false,
                    false => {
                        *self.locked.get() = true;
                        true
                    }
                });
                if acquired {
                    break;
                }
                asm!("wfi");
            }
        }
    }

    /// Releases the lock.
    pub fn unlock(&self) {
        // TODO: Use the core::intrinsics::coreatomic_cxchg_* functions once the MMU is enabled.
        // SAFETY: Safe on a single core because interrupts are disabled.
        unsafe {
            InterruptMask::instance().call(|| {
                *self.locked.get() = false;
            });
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

unsafe impl Sync for Lock {}
