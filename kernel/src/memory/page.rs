use core::mem::size_of;
use core::ops::Range;
use core::ptr;

use alloc::vec::Vec;

use crate::memory::{Page, PhysicalAddress, MEMORY_MAPPER};
use crate::sync::Mutex;

/// A simple page allocator.
///
/// Allocations are made linearly. Deallocations are not implemented.
pub struct PageAllocator {
    capacity: usize,
    reserved_ranges: Vec<Range<usize>>,
    allocated: Mutex<usize>,
}

impl PageAllocator {
    /// Creates a page allocator.
    pub fn new(capacity: usize, reserved_ranges: Vec<Range<PhysicalAddress<u8>>>) -> Self {
        let reserved_ranges = reserved_ranges
            .iter()
            .map(|v| v.start.address()..v.end.address())
            .collect();

        Self {
            capacity,
            reserved_ranges,
            allocated: Mutex::new(0),
        }
    }

    /// Allocates a page of memory.
    pub unsafe fn alloc(&'static self) -> Result<PageAllocation, ()> {
        let mut allocated = self.allocated.lock();
        let mut alloc_start;

        'outer: loop {
            // Record the starting point of this allocation.
            alloc_start = *allocated;

            // Reserve space to accommodate the page size.
            *allocated += size_of::<Page>();

            // Return an error if memory is exhausted.
            if *allocated > self.capacity {
                return Err(());
            }

            // Retry if this allocation intersects a reserved range.
            let alloc_range = alloc_start..(alloc_start + size_of::<Page>());
            for reserved_range in &self.reserved_ranges {
                if alloc_range.contains(&reserved_range.start)
                    || alloc_range.contains(&(reserved_range.end - 1))
                    || reserved_range.contains(&alloc_range.start)
                    || reserved_range.contains(&(alloc_range.end - 1))
                {
                    continue 'outer;
                }
            }

            break;
        }

        let addr = PhysicalAddress::new(alloc_start);
        let ptr = MEMORY_MAPPER.virtual_address(addr);

        // Zero out the page.
        ptr::write_bytes(ptr, 0, 1);

        Ok(PageAllocation::new(addr, ptr))
    }

    /// Deallocates a page of memory.
    pub unsafe fn dealloc(&self, page: &mut PageAllocation) {
        // Flag the allocation so that re-use can be detected.
        ptr::write_bytes(page.ptr, 0xDE, 1);

        // TODO: Implement deallocation.

        page.set_deallocated();
    }
}

/// An allocated page of physical memory.
pub struct PageAllocation {
    address: PhysicalAddress<Page>,
    ptr: *mut Page,
    allocated: Mutex<bool>,
}

impl PageAllocation {
    /// Creates a page allocation.
    fn new(address: PhysicalAddress<Page>, ptr: *mut Page) -> Self {
        PageAllocation {
            address,
            ptr,
            allocated: Mutex::new(true),
        }
    }

    /// Gets the physical address of the allocated page.
    pub fn address(&self) -> Option<PhysicalAddress<Page>> {
        if *self.allocated.lock() {
            Some(self.address)
        } else {
            None
        }
    }

    /// Gets the virtual address of the allocated page.
    pub fn page(&self) -> Option<*mut Page> {
        if *self.allocated.lock() {
            Some(self.ptr)
        } else {
            None
        }
    }

    /// Sets the page allocation as deallocated.
    fn set_deallocated(&mut self) {
        *self.allocated.lock() = false;
    }
}

impl Drop for PageAllocation {
    /// Drops the page allocation.
    fn drop(&mut self) {
        assert!(!*self.allocated.lock());
    }
}

unsafe impl Send for PageAllocation {}
unsafe impl Sync for PageAllocation {}
