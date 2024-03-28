use core::alloc::{Allocator, Layout};
use core::cmp::Ordering;
use core::marker::{PhantomData, Unsize};
use core::mem::{self, MaybeUninit};
use core::ops::{CoerceUnsized, Deref, DispatchFromDyn};
use core::ptr::{self, NonNull};

use alloc::alloc::Global;
use alloc::boxed::Box;

use crate::sync::Mutex;

/// A thread-safe reference-counting pointer.
///
/// This is a simplified version of `Arc` from the Rust Standard Library.
pub struct Arc<T: ?Sized> {
    ptr: NonNull<ArcInner<T>>,
    phantom: PhantomData<ArcInner<T>>,
}

impl<T> Arc<T> {
    /// Creates a reference-counting pointer.
    pub fn new(data: T) -> Self {
        // Construct the inner datum with one strong reference (the one created here) and one weak
        // reference (which is implicitly owned by all strong references).
        let inner = Box::new(ArcInner {
            strong: Mutex::new(1),
            weak: Mutex::new(1),
            data,
        });

        Self {
            ptr: Box::leak(inner).into(),
            phantom: PhantomData,
        }
    }

    /// Constructs a new `Arc<T>` while giving a `Weak<T>` to the allocation.
    pub fn new_cyclic<F>(data_func: F) -> Self
    where
        F: FnOnce(&Weak<T>) -> T,
    {
        // Construct the inner datum in an uninitialized state and with one weak reference (which
        // is passed to `data_func`).
        let inner = Box::new(ArcInner {
            strong: Mutex::new(0),
            weak: Mutex::new(1),
            data: MaybeUninit::<T>::uninit(),
        });
        let uninit_ptr: NonNull<_> = Box::leak(inner).into();
        let init_ptr: NonNull<ArcInner<T>> = uninit_ptr.cast();

        let weak = Weak { ptr: init_ptr };
        let data = data_func(&weak);

        // Initialize the inner value.
        let strong = unsafe {
            let inner = init_ptr.as_ptr();
            ptr::write(ptr::addr_of_mut!((*inner).data), data);

            // Begin converting the weak value into a strong value.
            *init_ptr.as_ref().strong.lock() += 1;

            Self {
                ptr: init_ptr,
                phantom: PhantomData,
            }
        };

        // Strong references should collectively own one weak reference, so the destructor for the
        // transient weak reference is skipped.
        mem::forget(weak);
        strong
    }
}

impl<T: ?Sized> Arc<T> {
    /// Creates a new `Weak` pointer to this allocation.
    pub fn downgrade(this: &Self) -> Weak<T> {
        let inner = unsafe { this.ptr.as_ref() };
        let mut weak = inner.weak.lock();

        *weak += 1;

        Weak { ptr: this.ptr }
    }
}

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<Arc<U>> for Arc<T> {}
impl<T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<Arc<U>> for Arc<T> {}

impl<T: ?Sized> Clone for Arc<T> {
    fn clone(&self) -> Arc<T> {
        let inner = unsafe { self.ptr.as_ref() };
        let mut strong = inner.strong.lock();

        *strong += 1;

        Self {
            ptr: self.ptr,
            phantom: PhantomData,
        }
    }
}

impl<T: ?Sized> Drop for Arc<T> {
    fn drop(&mut self) {
        let inner = unsafe { self.ptr.as_ref() };
        let mut strong = inner.strong.lock();

        *strong -= 1;

        if *strong == 0 {
            drop(strong);

            // SAFETY: The inner datum (excluding counts) is no longer used.
            unsafe { ptr::drop_in_place(&mut (*self.ptr.as_ptr()).data) };

            // Drop the weak reference collectively held by all strong references.
            drop(Weak { ptr: self.ptr });
        }
    }
}

impl<T: ?Sized> Deref for Arc<T> {
    type Target = T;

    fn deref(&self) -> &T {
        unsafe { &self.ptr.as_ref().data }
    }
}

impl<T: ?Sized + PartialEq> PartialEq for Arc<T> {
    fn eq(&self, other: &Self) -> bool {
        ptr::addr_eq(self.ptr.as_ptr(), other.ptr.as_ptr()) || **self == **other
    }

    fn ne(&self, other: &Self) -> bool {
        !ptr::addr_eq(self.ptr.as_ptr(), other.ptr.as_ptr()) && **self != **other
    }
}

impl<T: ?Sized + Eq> Eq for Arc<T> {}

impl<T: ?Sized + PartialOrd> PartialOrd for Arc<T> {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        (**self).partial_cmp(&**other)
    }

    fn lt(&self, other: &Arc<T>) -> bool {
        (**self).lt(&**other)
    }

    fn le(&self, other: &Arc<T>) -> bool {
        (**self).le(&**other)
    }

    fn gt(&self, other: &Arc<T>) -> bool {
        (**self).gt(&**other)
    }

    fn ge(&self, other: &Arc<T>) -> bool {
        (**self).ge(&**other)
    }
}

impl<T: ?Sized + Ord> Ord for Arc<T> {
    fn cmp(&self, other: &Arc<T>) -> Ordering {
        (**self).cmp(&**other)
    }
}

unsafe impl<T: ?Sized + Send + Sync> Send for Arc<T> {}
unsafe impl<T: ?Sized + Send + Sync> Sync for Arc<T> {}

/// `Weak` is a version of `Arc` that holds a non-owning reference to the managed allocation
///
/// This is a simplified version of `Weak` from the Rust Standard Library.
pub struct Weak<T: ?Sized> {
    ptr: NonNull<ArcInner<T>>,
}

impl<T: ?Sized> Weak<T> {
    /// Attempts to upgrade the `Weak` pointer to an `Arc`.
    pub fn upgrade(&self) -> Option<Arc<T>> {
        let inner = unsafe { self.ptr.as_ref() };
        let mut strong = inner.strong.lock();

        if *strong == 0 {
            return None;
        }

        *strong += 1;

        Some(Arc {
            ptr: self.ptr,
            phantom: PhantomData,
        })
    }
}

impl<T: ?Sized + Unsize<U>, U: ?Sized> CoerceUnsized<Weak<U>> for Weak<T> {}
impl<T: ?Sized + Unsize<U>, U: ?Sized> DispatchFromDyn<Weak<U>> for Weak<T> {}

impl<T: ?Sized> Clone for Weak<T> {
    fn clone(&self) -> Weak<T> {
        let inner = unsafe { self.ptr.as_ref() };
        let mut weak = inner.weak.lock();

        *weak += 1;

        Self { ptr: self.ptr }
    }
}

impl<T: ?Sized> Drop for Weak<T> {
    fn drop(&mut self) {
        let inner = unsafe { self.ptr.as_ref() };
        let strong = inner.strong.lock();
        let mut weak = inner.weak.lock();

        *weak -= 1;

        if *strong == 0 && *weak == 0 {
            drop(strong);
            drop(weak);

            // SAFETY: The inner datum (including counts) is no longer used.
            unsafe {
                Global.deallocate(self.ptr.cast(), Layout::for_value_raw(self.ptr.as_ptr()));
            }
        }
    }
}

unsafe impl<T: ?Sized + Send + Sync> Send for Weak<T> {}
unsafe impl<T: ?Sized + Send + Sync> Sync for Weak<T> {}

/// The inner datum that is shared between instances.
struct ArcInner<T: ?Sized> {
    strong: Mutex<usize>,
    weak: Mutex<usize>,
    data: T,
}

unsafe impl<T: ?Sized + Send + Sync> Send for ArcInner<T> {}
unsafe impl<T: ?Sized + Send + Sync> Sync for ArcInner<T> {}
