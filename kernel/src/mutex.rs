use core::cell::UnsafeCell;
use core::ops::{Deref, DerefMut};

use crate::arch::lock::Lock;

/// A mutual exclusion primitive for protecting shared data
///
/// This is a simplified version of `Mutex` from the Rust Standard Library.
///
/// # Examples
///
/// ```
/// use crate::mutex::Mutex;
///
/// static SHARED: Mutex<u32> = Mutex::new(123);
///
/// println!("{}", *SHARED.lock());
/// *SHARED.lock() = 456;
/// println!("{}", *SHARED.lock());
/// ```
pub struct Mutex<T> {
    data: UnsafeCell<T>,
    inner: Lock,
}

impl<T> Mutex<T> {
    pub const fn new(data: T) -> Self {
        Self {
            data: UnsafeCell::new(data),
            inner: Lock::new(),
        }
    }

    pub fn lock(&self) -> MutexGuard<'_, T> {
        self.inner.lock();
        MutexGuard::new(self)
    }
}

unsafe impl<T> Sync for Mutex<T> {}

/// An RAII implementation of a "scoped lock" of a mutex
///
/// This is a simplified version of `MutexGuard` from the Rust Standard Library.
pub struct MutexGuard<'a, T> {
    lock: &'a Mutex<T>,
}

impl<'a, T> MutexGuard<'a, T> {
    pub fn new(lock: &'a Mutex<T>) -> Self {
        Self { lock }
    }
}

impl<T> Drop for MutexGuard<'_, T> {
    fn drop(&mut self) {
        self.lock.inner.unlock();
    }
}

impl<T> Deref for MutexGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &*self.lock.data.get() }
    }
}

impl<T> DerefMut for MutexGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        unsafe { &mut *self.lock.data.get() }
    }
}
