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

impl Drop for Page {
    fn drop(&mut self) {
        // SAFETY: The caller must keep a reference to every page in use.
        unsafe {
            PageAllocator::instance().dealloc(self);
        }
    }
}

unsafe impl Send for Page {}
unsafe impl Sync for Page {}

/// A simple page allocator
///
/// Allocations are made linearly. Deallocations are not implemented.
pub struct PageAllocator {
    capacity: Mutex<usize>,
    reserved_ranges: Mutex<Vec<Range<usize>>>,
    allocated: Mutex<usize>,
}

impl PageAllocator {
    pub fn instance() -> &'static Self {
        static INSTANCE: PageAllocator = PageAllocator::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            capacity: Mutex::new(0),
            reserved_ranges: Mutex::new(Vec::new()),
            allocated: Mutex::new(0),
        }
    }

    pub unsafe fn set_capacity(&self, capacity: usize) {
        *self.capacity.lock() = capacity;
    }

    pub unsafe fn add_reserved_range(&self, range: Range<usize>) {
        self.reserved_ranges.lock().push(range);
    }

    pub unsafe fn alloc(&self) -> Page {
        let capacity = self.capacity.lock();
        let reserved = self.reserved_ranges.lock();
        let mut allocated = self.allocated.lock();
        let mut alloc_start;

        'outer: loop {
            // Record the starting point of this allocation.
            alloc_start = *allocated;

            // Reserve space to accommodate the page size.
            *allocated += PAGE_SIZE;

            // Return a null pointer if memory is exhausted.
            if *allocated > *capacity {
                panic!("page allocation failed");
            }

            // Retry if this allocation intersects a reserved range.
            let alloc_range = alloc_start..(alloc_start + PAGE_SIZE);
            for reserved_range in &*reserved {
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
