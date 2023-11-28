use core::ops::Range;
use core::ptr;

use alloc::vec::Vec;

use crate::sync::Mutex;

/// A simple page allocator.
///
/// Allocations are made linearly. Deallocations are not implemented.
pub struct PageAllocator {
    page_size: usize,
    capacity: usize,
    reserved_ranges: Vec<Range<usize>>,
    allocated: Mutex<usize>,
}

impl PageAllocator {
    pub fn new(page_size: usize, capacity: usize, reserved_ranges: Vec<Range<usize>>) -> Self {
        Self {
            page_size,
            capacity,
            reserved_ranges,
            allocated: Mutex::new(0),
        }
    }

    pub unsafe fn alloc(&self) -> Page {
        let mut allocated = self.allocated.lock();
        let mut alloc_start;

        'outer: loop {
            // Record the starting point of this allocation.
            alloc_start = *allocated;

            // Reserve space to accommodate the page size.
            *allocated += self.page_size;

            // Return a null pointer if memory is exhausted.
            if *allocated > self.capacity {
                panic!("page allocation failed");
            }

            // Retry if this allocation intersects a reserved range.
            let alloc_range = alloc_start..(alloc_start + self.page_size);
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

        // Zero out the page.
        ptr::write_bytes(alloc_start as *mut u8, 0, self.page_size);

        Page::new(alloc_start as *mut u8, self.page_size)
    }

    pub unsafe fn dealloc(&self, _page: &mut Page) {
        // TODO: Implement deallocation.
    }
}

/// An allocated page of physical memory.
pub struct Page {
    start: *mut u8,
    size: usize,
}

impl Page {
    fn new(start: *mut u8, size: usize) -> Self {
        Page { start, size }
    }

    pub fn start(&self) -> *mut u8 {
        self.start
    }

    pub fn end_exclusive(&self) -> *mut u8 {
        // SAFETY: Usage of this pointer is also unsafe.
        unsafe { self.start.add(self.size) }
    }

    pub fn size(&self) -> usize {
        self.size
    }
}

unsafe impl Send for Page {}
unsafe impl Sync for Page {}
