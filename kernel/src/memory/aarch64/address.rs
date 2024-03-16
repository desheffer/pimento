use core::marker::PhantomData;

use crate::memory::VA_START;

/// An AArch64 physical address.
///
/// This is used to differentiate physical addresses from virtual addresses.
pub struct PhysicalAddress<T> {
    address: usize,
    phantom: PhantomData<*const T>,
}

impl<T> PhysicalAddress<T> {
    /// Creates a physical address.
    pub fn new(address: usize) -> Self {
        Self {
            address,
            phantom: PhantomData,
        }
    }

    /// Creates a physical address from a pointer in kernel virtual memory.
    pub fn from_ptr(ptr: *const T) -> Self {
        let address = (ptr as usize) & !VA_START;
        Self::new(address)
    }

    /// Gets the physical address as a pointer in kernel virtual memory.
    pub unsafe fn as_ptr(&self) -> *const T {
        (self.address | VA_START) as *const T
    }

    /// Gets the physical address as a mutable pointer in kernel virtual memory.
    pub unsafe fn as_mut_ptr(&self) -> *mut T {
        (self.address | VA_START) as *mut T
    }
}

impl<T> Clone for PhysicalAddress<T> {
    fn clone(&self) -> Self {
        *self
    }
}

impl<T> Copy for PhysicalAddress<T> {}

impl<T> From<PhysicalAddress<T>> for usize {
    fn from(value: PhysicalAddress<T>) -> Self {
        value.address
    }
}

impl<T> From<PhysicalAddress<T>> for u64 {
    fn from(value: PhysicalAddress<T>) -> Self {
        value.address as u64
    }
}
