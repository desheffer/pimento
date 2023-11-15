use core::borrow::Borrow;
use core::marker::Unsize;
use core::ops::{CoerceUnsized, Deref, DispatchFromDyn};
use core::ptr::NonNull;

use alloc::boxed::Box;

use crate::sync::Mutex;

/// A thread-safe reference-counting pointer
///
/// This is a simplified version of `Arc` from the Rust Standard Library.
#[derive(Debug)]
pub struct Arc<T: ?Sized> {
    ptr: NonNull<ArcInner<T>>,
}

impl<T> Arc<T> {
    pub fn new(data: T) -> Self {
        let inner = Box::new(ArcInner {
            strong: Mutex::new(1),
            data,
        });

        Self {
            ptr: Box::leak(inner).into(),
        }
    }
}

impl<T: ?Sized> Arc<T> {
    fn inner(&self) -> &ArcInner<T> {
        // SAFETY: Safe because this is a managed pointer.
        unsafe { self.ptr.as_ref() }
    }
}

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<Arc<U>> for Arc<T> {}
impl<T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<Arc<U>> for Arc<T> {}

impl<T: ?Sized> Clone for Arc<T> {
    fn clone(&self) -> Arc<T> {
        let mut strong = self.inner().strong.lock();
        *strong += 1;

        Self { ptr: self.ptr }
    }
}

impl<T: ?Sized> Drop for Arc<T> {
    fn drop(&mut self) {
        let mut strong = self.inner().strong.lock();
        *strong -= 1;

        if *strong == 0 {
            // SAFETY: The inner datum is no longer used.
            unsafe {
                let inner = Box::from_raw(self.ptr.as_ptr());
                drop(inner);
            }
        }
    }
}

impl<T: ?Sized> Deref for Arc<T> {
    type Target = T;

    fn deref(&self) -> &T {
        &self.inner().data
    }
}

impl<T: ?Sized> Borrow<T> for Arc<T> {
    fn borrow(&self) -> &T {
        &self.inner().data
    }
}

unsafe impl<T: ?Sized + Send + Sync> Send for Arc<T> {}
unsafe impl<T: ?Sized + Send + Sync> Sync for Arc<T> {}

/// The inner datum that is shared
#[derive(Debug)]
struct ArcInner<T: ?Sized> {
    strong: Mutex<usize>,
    data: T,
}
