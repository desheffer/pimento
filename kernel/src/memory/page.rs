use core::ops::Range;

use alloc::vec::Vec;

use crate::sync::Mutex;

const PAGE_SIZE: usize = 4096;

/// An aligned page of physical memory
#[derive(Debug)]
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

/// A simple page allocator
///
/// Allocations are made linearly. Deallocations are not implemented.
#[derive(Debug)]
pub struct PageAllocator {
    capacity: usize,
    reserved_ranges: Vec<Range<usize>>,
    allocated: Mutex<usize>,
}

impl PageAllocator {
    pub fn new(capacity: usize, reserved_ranges: Vec<Range<usize>>) -> Self {
        Self {
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
            *allocated += PAGE_SIZE;

            // Return a null pointer if memory is exhausted.
            if *allocated > self.capacity {
                panic!("page allocation failed");
            }

            // Retry if this allocation intersects a reserved range.
            let alloc_range = alloc_start..(alloc_start + PAGE_SIZE);
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

        Page::new(alloc_start as *mut u8, PAGE_SIZE)
    }

    pub unsafe fn dealloc(&self, _page: &mut Page) {} // TODO
}
