use core::cell::UnsafeCell;
use core::mem::MaybeUninit;

use crate::sync::Once;

/// A synchronization primitive which can be written to only once.
///
/// This is a simplified version of `OnceLock` from the Rust Standard Library.
#[derive(Debug)]
pub struct OnceLock<T> {
    once: Once,
    data: UnsafeCell<MaybeUninit<T>>,
}

impl<T> OnceLock<T> {
    pub const fn new() -> Self {
        Self {
            once: Once::new(),
            data: UnsafeCell::new(MaybeUninit::uninit()),
        }
    }

    pub fn get(&self) -> Option<&T> {
        // SAFETY: Safe because call is behind a lock.
        unsafe {
            if self.is_initialized() {
                Some((*self.data.get()).assume_init_ref())
            } else {
                None
            }
        }
    }

    pub fn set(&self, value: T) -> Result<(), T> {
        let mut value = Some(value);
        self.get_or_init(|| value.take().unwrap());
        match value {
            None => Ok(()),
            Some(value) => Err(value),
        }
    }

    pub fn get_or_init<F>(&self, f: F) -> &T
    where
        F: FnOnce() -> T,
    {
        // SAFETY: Safe because call is behind a lock.
        unsafe {
            self.once.call_once(|| {
                (*self.data.get()).write(f());
            });

            (*self.data.get()).assume_init_ref()
        }
    }

    pub fn is_initialized(&self) -> bool {
        self.once.is_completed()
    }
}

unsafe impl<T> Sync for OnceLock<T> {}
