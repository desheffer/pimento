use alloc::vec::Vec;

use crate::sync::{Mutex, OnceLock};

const PAGE_SIZE: usize = 4096;

/// An arbitrary range of memory
#[derive(Debug)]
struct Range {
    start: *mut u8,
    size: usize,
}

impl Range {
    fn new(start: *mut u8, size: usize) -> Self {
        Self { start, size }
    }

    pub fn start(&self) -> *mut u8 {
        self.start
    }

    pub fn size(&self) -> usize {
        self.size
    }
}

/// An aligned page of physical memory
#[derive(Debug)]
pub struct Page {
    range: Range,
}

impl Page {
    fn new(start: *mut u8, size: usize) -> Self {
        Page {
            range: Range::new(start, size),
        }
    }

    pub fn start(&self) -> *mut u8 {
        self.range.start()
    }

    pub fn end_exclusive(&self) -> *mut u8 {
        // SAFETY: Usage of this pointer is also unsafe.
        unsafe { self.start().add(self.size()) }
    }

    pub fn size(&self) -> usize {
        self.range.size()
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

/// A simple page allocator
///
/// Allocations are made linearly. Deallocations are not implemented.
pub struct PageAllocator {
    pages: Mutex<Vec<Range>>,
    reserved: Mutex<usize>,
}

impl PageAllocator {
    pub fn instance() -> &'static Self {
        static INSTANCE: OnceLock<PageAllocator> = OnceLock::new();
        INSTANCE.get_or_init(|| Self::new())
    }

    fn new() -> Self {
        Self {
            pages: Mutex::new(Vec::new()),
            reserved: Mutex::new(0),
        }
    }

    pub unsafe fn add_capacity(&self, start: *mut u8, size: usize) {
        assert!(start as usize % PAGE_SIZE == 0);

        // TODO: Multiple ranges are not currently supported.
        assert!(self.pages.lock().is_empty());

        self.pages.lock().push(Range::new(start, size));
    }

    pub unsafe fn alloc(&self) -> Page {
        let pages = self.pages.lock();
        let mut reserved = self.reserved.lock();

        assert!(!pages.is_empty());

        let alloc_start = pages[0].start.add(*reserved);

        *reserved += PAGE_SIZE;

        Page::new(alloc_start, PAGE_SIZE)
    }

    pub unsafe fn dealloc(&self, _page: &mut Page) {} // TODO
}
