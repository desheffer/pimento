use core::cell::UnsafeCell;

use crate::sync::Lock;

/// A synchronization primitive which can be used to run a one-time global initialization.
///
/// This is a simplified version of `Once` from the Rust Standard Library.
pub struct Once {
    lock: Lock,
    completed: UnsafeCell<bool>,
}

impl Once {
    /// Creates a one-time global initialization.
    pub const fn new() -> Self {
        Self {
            lock: Lock::new(),
            completed: UnsafeCell::new(false),
        }
    }

    /// Performs an initialization routine once and only once. The given closure will be executed
    /// if this is the first time `call_once` has been called, and otherwise the routine will not
    /// be invoked.
    pub fn call_once<F>(&self, f: F)
    where
        F: FnOnce(),
    {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            if !*self.completed.get() {
                f();
                *self.completed.get() = true;
            }
        })
    }

    /// Returns `true` if some `call_once()` call has completed successfully.
    pub fn is_completed(&self) -> bool {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe { *self.completed.get() })
    }
}

unsafe impl Sync for Once {}
