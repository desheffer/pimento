use core::marker::PhantomData;

/// A physical address.
///
/// This is used to differentiate a physical address from a virtual address. A physical address
/// exists in the hardware, but there is no guarantee that it is mapped in virtual memory.
#[derive(Clone, Copy)]
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

    /// Gets the physical address value.
    pub fn address(&self) -> usize {
        self.address
    }

    /// Converts the type of the physical address.
    pub fn convert<U>(&self) -> PhysicalAddress<U> {
        PhysicalAddress::<U>::new(self.address)
    }
}
