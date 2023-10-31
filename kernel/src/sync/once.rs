use core::cell::UnsafeCell;

use crate::sync::Lock;

/// A synchronization primitive which can be used to run a one-time global initialization.
///
/// This is a simplified version of `Once` from the Rust Standard Library.
#[derive(Debug)]
pub struct Once {
    lock: Lock,
    completed: UnsafeCell<bool>,
}

impl Once {
    pub const fn new() -> Self {
        Self {
            lock: Lock::new(),
            completed: UnsafeCell::new(false),
        }
    }

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

    pub fn is_completed(&self) -> bool {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe { *self.completed.get() })
    }
}

unsafe impl Sync for Once {}
