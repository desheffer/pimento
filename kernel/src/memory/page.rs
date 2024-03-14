use core::mem::size_of;
use core::ops::Range;
use core::ptr;

use alloc::vec::Vec;

use crate::memory::{Page, PhysicalAddress};
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
            .map(|v| v.start.into()..v.end.into())
            .collect();

        Self {
            capacity,
            reserved_ranges,
            allocated: Mutex::new(0),
        }
    }

    /// Allocates a page of memory.
    pub unsafe fn alloc(&'static self) -> PageAllocation {
        let mut allocated = self.allocated.lock();
        let mut alloc_start;

        'outer: loop {
            // Record the starting point of this allocation.
            alloc_start = *allocated;

            // Reserve space to accommodate the page size.
            *allocated += size_of::<Page>();

            // Return a null pointer if memory is exhausted.
            if *allocated > self.capacity {
                panic!("page allocation failed");
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

        let page = PageAllocation::new(PhysicalAddress::new(alloc_start));

        // Zero out the page.
        ptr::write_bytes(page.as_mut_ptr(), 0, 1);

        page
    }

    /// Deallocates a page of memory.
    pub unsafe fn dealloc(&self, page: &mut PageAllocation) {
        // Flag the allocation so that re-use can be detected.
        ptr::write_bytes(page.as_mut_ptr(), 0xDE, 1);

        // TODO: Implement deallocation.

        page.set_deallocated();
    }
}

/// An allocated page of physical memory.
pub struct PageAllocation {
    page: PhysicalAddress<Page>,
    allocated: Mutex<bool>,
}

impl PageAllocation {
    /// Creates a page allocation.
    fn new(page: PhysicalAddress<Page>) -> Self {
        PageAllocation {
            page,
            allocated: Mutex::new(true),
        }
    }

    /// Gets the physical address of the allocated page.
    pub fn addr(&self) -> PhysicalAddress<Page> {
        assert!(*self.allocated.lock());
        self.page
    }

    /// Gets the size of the allocated page.
    pub const fn size(&self) -> usize {
        size_of::<Page>()
    }

    /// Gets the allocated page as a pointer in kernel virtual memory.
    pub unsafe fn as_ptr(&self) -> *const Page {
        assert!(*self.allocated.lock());
        self.page.as_ptr()
    }

    /// Gets the allocated page as a mutable pointer in kernel virtual memory.
    pub unsafe fn as_mut_ptr(&self) -> *mut Page {
        assert!(*self.allocated.lock());
        self.page.as_mut_ptr()
    }

    /// Sets the page allocation as deallocated.
    fn set_deallocated(&mut self) {
        let mut allocated = self.allocated.lock();
        assert!(*allocated);
        *allocated = false;
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
