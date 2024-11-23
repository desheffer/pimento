use core::marker::PhantomData;
use core::mem::size_of;

/// A physical address.
///
/// This is used to differentiate a physical address from a virtual address. A physical address
/// exists in the hardware, but there is no guarantee that it is mapped in virtual memory.
pub struct PhysicalAddress<T> {
    address: usize,
    phantom: PhantomData<*mut T>,
}

impl<T> PhysicalAddress<T> {
    /// Creates a physical address.
    pub fn new(address: usize) -> Self {
        Self {
            address,
            phantom: PhantomData,
        }
    }

    /// Gets the physical address value.
    pub fn address(&self) -> usize {
        self.address
    }

    /// Converts the type of the physical address.
    pub fn convert<U>(&self) -> PhysicalAddress<U> {
        PhysicalAddress::<U>::new(self.address)
    }
}

impl<T> Clone for PhysicalAddress<T> {
    fn clone(&self) -> Self {
        *self
    }
}

impl<T> Copy for PhysicalAddress<T> {}

/// A user-space virtual address.
///
/// This is used to differentiate a user-space virtual address from a kernel-space virtual address.
/// A user-space virtual address is only valid in a specific context and is presumed to require
/// additional scrutiny.
pub struct UserAddress<T> {
    address: usize,
    phantom: PhantomData<*mut T>,
}

impl<T> UserAddress<T> {
    /// Creates a user virtual address.
    pub fn new(address: usize) -> Self {
        Self {
            address,
            phantom: PhantomData,
        }
    }

    /// Converts the type of the user virtual address.
    pub fn convert<U>(&self) -> UserAddress<U> {
        UserAddress::<U>::new(self.address)
    }

    /// Gets the user virtual address as a pointer.
    pub unsafe fn as_ptr(&self) -> *mut T {
        self.address as *mut T
    }

    /// Calculates the offset from a pointer.
    pub unsafe fn add(&self, count: usize) -> Self {
        Self {
            address: self.address + size_of::<T>() * count,
            phantom: PhantomData,
        }
    }

    /// Calculates the offset from a pointer.
    pub unsafe fn sub(&self, count: usize) -> Self {
        Self {
            address: self.address - size_of::<T>() * count,
            phantom: PhantomData,
        }
    }
}

impl<T> Clone for UserAddress<T> {
    fn clone(&self) -> Self {
        *self
    }
}

impl<T> Copy for UserAddress<T> {}
