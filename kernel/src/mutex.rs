use core::borrow::{Borrow, BorrowMut};
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
#[derive(Debug)]
pub struct Mutex<T> {
    data: UnsafeCell<T>,
    lock: Lock,
}

impl<T> Mutex<T> {
    pub const fn new(data: T) -> Self {
        Self {
            data: UnsafeCell::new(data),
            lock: Lock::new(),
        }
    }

    pub fn lock(&self) -> MutexGuard<'_, T> {
        self.lock.lock();
        MutexGuard::new(self)
    }
}

unsafe impl<T: Send> Send for Mutex<T> {}
unsafe impl<T: Send> Sync for Mutex<T> {}

/// An RAII implementation of a "scoped lock" of a mutex
///
/// This is a simplified version of `MutexGuard` from the Rust Standard Library.
#[derive(Debug)]
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
        self.lock.lock.unlock();
    }
}

impl<T> Deref for MutexGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &*self.lock.data.get() }
    }
}

impl<T> DerefMut for MutexGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T> Borrow<T> for MutexGuard<'_, T> {
    fn borrow(&self) -> &T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &*self.lock.data.get() }
    }
}

impl<T> BorrowMut<T> for MutexGuard<'_, T> {
    fn borrow_mut(&mut self) -> &mut T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &mut *self.lock.data.get() }
    }
}

unsafe impl<T: Sync> Sync for MutexGuard<'_, T> {}
