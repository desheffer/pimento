use core::hint::spin_loop;
use core::sync::atomic::AtomicBool;
use core::sync::atomic::Ordering::{Acquire, Relaxed, Release};

/// A simple spin lock.
pub struct Lock {
    locked: AtomicBool,
}

impl Lock {
    /// Creates a lock.
    pub const fn new() -> Self {
        Self {
            locked: AtomicBool::new(false),
        }
    }

    /// Obtains the lock.
    pub fn lock(&self) {
        // Spin until the lock is acquired.
        loop {
            if let Ok(_) = self.locked.compare_exchange(false, true, Acquire, Relaxed) {
                break;
            }
            spin_loop();
        }
    }

    /// Releases the lock.
    pub fn unlock(&self) {
        self.locked.store(false, Release);
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
