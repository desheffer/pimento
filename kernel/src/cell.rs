use core::cell::UnsafeCell;
use core::mem::MaybeUninit;

use crate::arch::lock::Lock;

/// A synchronization primitive which can be written to only once.
///
/// This is a simplified version of `OnceLock` from the Rust Standard Library.
#[derive(Debug)]
pub struct OnceLock<T> {
    lock: Lock,
    initialized: UnsafeCell<bool>,
    data: UnsafeCell<MaybeUninit<T>>,
}

impl<T> OnceLock<T> {
    pub const fn new() -> Self {
        Self {
            lock: Lock::new(),
            initialized: UnsafeCell::new(false),
            data: UnsafeCell::new(MaybeUninit::uninit()),
        }
    }

    pub fn get_or_init<F>(&self, f: F) -> &T
    where
        F: FnOnce() -> T,
    {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            if !*self.initialized.get() {
                (*self.data.get()).write(f());
                *self.initialized.get() = true;
            }

            (*self.data.get()).assume_init_ref()
        })
    }
}

unsafe impl<T> Sync for OnceLock<T> {}
