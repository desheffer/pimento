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
    pub fn new(capacity: usize, reserved_ranges: &[Range<PhysicalAddress<u8>>]) -> Self {
        let reserved_ranges = reserved_ranges
            .iter()
            .map(|range| range.start.address()..range.end.address())
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

        Ok(PageAllocation::new(self, addr, ptr))
    }

    /// Deallocates a page of memory.
    unsafe fn dealloc(&self, page: &mut PageAllocation) {
        // Clear the allocation so that re-use can be detected.
        ptr::write_bytes(page.ptr, 0x00, 1);

        // TODO: Implement deallocation.
    }
}

/// An allocated page of physical memory.
pub struct PageAllocation {
    page_allocator: &'static PageAllocator,
    address: PhysicalAddress<Page>,
    ptr: *mut Page,
}

impl PageAllocation {
    /// Creates a page allocation.
    fn new(
        page_allocator: &'static PageAllocator,
        address: PhysicalAddress<Page>,
        ptr: *mut Page,
    ) -> Self {
        PageAllocation {
            page_allocator,
            address,
            ptr,
        }
    }

    /// Gets the physical address of the allocated page.
    pub fn address(&self) -> PhysicalAddress<Page> {
        self.address
    }

    /// Gets the virtual address of the allocated page.
    pub fn page(&self) -> *mut Page {
        self.ptr
    }
}

impl Drop for PageAllocation {
    fn drop(&mut self) {
        unsafe {
            self.page_allocator.dealloc(self);
        }
    }
}
