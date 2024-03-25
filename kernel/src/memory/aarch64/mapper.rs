use crate::memory::{PhysicalAddress, VA_START};

pub static MEMORY_MAPPER: MemoryMapper = MemoryMapper::new();

/// A mapper between physical memory addresses and kernel virtual memory addresses.
pub struct MemoryMapper {}

impl MemoryMapper {
    /// Creates a memory mapper.
    const fn new() -> Self {
        Self {}
    }

    /// Converts a physical address to a virtual address.
    pub fn virtual_address<T>(&self, address: PhysicalAddress<T>) -> *mut T {
        (address.address() | VA_START) as *mut T
    }

    /// Converts a virtual address to a physical address.
    pub fn physical_address<T>(&self, ptr: *const T) -> PhysicalAddress<T> {
        PhysicalAddress::new((ptr as usize) & !VA_START)
    }
}
