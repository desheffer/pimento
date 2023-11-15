use core::borrow::{Borrow, BorrowMut};
use core::cell::UnsafeCell;
use core::ops::{Deref, DerefMut};

use crate::sync::Lock;

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
pub struct Mutex<T: ?Sized> {
    lock: Lock,
    data: UnsafeCell<T>,
}

impl<T> Mutex<T> {
    pub const fn new(data: T) -> Self {
        Self {
            data: UnsafeCell::new(data),
            lock: Lock::new(),
        }
    }
}

impl<T: ?Sized> Mutex<T> {
    pub fn lock(&self) -> MutexGuard<'_, T> {
        self.lock.lock();
        MutexGuard::new(self)
    }
}

unsafe impl<T: ?Sized + Send> Send for Mutex<T> {}
unsafe impl<T: ?Sized + Send> Sync for Mutex<T> {}

/// An RAII implementation of a "scoped lock" of a mutex
///
/// This is a simplified version of `MutexGuard` from the Rust Standard Library.
#[derive(Debug)]
pub struct MutexGuard<'a, T: ?Sized> {
    lock: &'a Mutex<T>,
}

impl<'a, T: ?Sized> MutexGuard<'a, T> {
    pub fn new(lock: &'a Mutex<T>) -> Self {
        Self { lock }
    }
}

impl<T: ?Sized> Drop for MutexGuard<'_, T> {
    fn drop(&mut self) {
        self.lock.lock.unlock();
    }
}

impl<T: ?Sized> Deref for MutexGuard<'_, T> {
    type Target = T;

    fn deref(&self) -> &T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &*self.lock.data.get() }
    }
}

impl<T: ?Sized> DerefMut for MutexGuard<'_, T> {
    fn deref_mut(&mut self) -> &mut T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &mut *self.lock.data.get() }
    }
}

impl<T: ?Sized> Borrow<T> for MutexGuard<'_, T> {
    fn borrow(&self) -> &T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &*self.lock.data.get() }
    }
}

impl<T: ?Sized> BorrowMut<T> for MutexGuard<'_, T> {
    fn borrow_mut(&mut self) -> &mut T {
        // SAFETY: Safe because data is behind a lock.
        unsafe { &mut *self.lock.data.get() }
    }
}

unsafe impl<T: ?Sized + Sync> Sync for MutexGuard<'_, T> {}
