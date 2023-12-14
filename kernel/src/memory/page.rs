use core::mem::size_of;
use core::ops::Range;
use core::ptr;

use alloc::vec::Vec;

use crate::memory::{Page, PhysicalAddress};
use crate::sync::{Mutex, OnceLock};

/// A simple page allocator.
///
/// Allocations are made linearly. Deallocations are not implemented.
pub struct PageAllocator {
    capacity: usize,
    reserved_ranges: Vec<Range<usize>>,
    allocated: Mutex<usize>,
}

static INSTANCE: OnceLock<PageAllocator> = OnceLock::new();
static INIT_CAPACITY: Mutex<Option<usize>> = Mutex::new(None);
static INIT_RESERVED_RANGES: Mutex<Option<Vec<Range<usize>>>> = Mutex::new(None);

impl PageAllocator {
    /// Sets the memory capacity for the system.
    pub unsafe fn set_capacity(capacity: usize) {
        assert!(INSTANCE.get().is_none());
        *INIT_CAPACITY.lock() = Some(capacity);
    }

    /// Sets ranges of memory as reserved.
    pub unsafe fn set_reserved_ranges(reserved_ranges: Vec<Range<PhysicalAddress<u8>>>) {
        assert!(INSTANCE.get().is_none());
        *INIT_RESERVED_RANGES.lock() = Some(
            reserved_ranges
                .iter()
                .map(|v| v.start.into()..v.end.into())
                .collect(),
        );
    }

    /// Gets or initializes the page allocator.
    pub fn instance() -> &'static Self {
        INSTANCE.get_or_init(|| {
            let capacity = INIT_CAPACITY
                .lock()
                .take()
                .expect("PageAllocator::set_capacity() was expected");
            let reserved_ranges = INIT_RESERVED_RANGES
                .lock()
                .take()
                .expect("PageAllocator::set_reserved_ranges() was expected");

            Self::new(capacity, reserved_ranges)
        })
    }

    fn new(capacity: usize, reserved_ranges: Vec<Range<usize>>) -> Self {
        Self {
            capacity,
            reserved_ranges,
            allocated: Mutex::new(0),
        }
    }

    /// Allocates a page. The page is zeroed out.
    pub unsafe fn alloc(&self) -> PageAllocation {
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

    /// Deallocates a page.
    pub unsafe fn dealloc(&self, page: &mut PageAllocation) {
        // Flag the allocation so that re-use can be detected.
        ptr::write_bytes(page.as_mut_ptr(), 0xDE, 1);

        // TODO: Implement deallocation.
    }
}

/// An allocated page of physical memory.
pub struct PageAllocation {
    page: PhysicalAddress<Page>,
}

impl PageAllocation {
    /// Creates a page allocation.
    fn new(page: PhysicalAddress<Page>) -> Self {
        PageAllocation { page }
    }

    /// Gets the physical address of the allocated page.
    pub fn addr(&self) -> PhysicalAddress<Page> {
        self.page
    }

    /// Gets the size of the allocated page.
    pub const fn size(&self) -> usize {
        size_of::<Page>()
    }

    /// Gets the allocated page as a pointer in kernel virtual memory.
    pub unsafe fn as_ptr(&self) -> *const Page {
        self.page.as_ptr()
    }

    /// Gets the allocated page as a mutable pointer in kernel virtual memory.
    pub unsafe fn as_mut_ptr(&self) -> *mut Page {
        self.page.as_mut_ptr()
    }
}

impl Drop for PageAllocation {
    fn drop(&mut self) {
        // SAFETY: Safe because the allocation was created above, but only if all pointers have
        // been discarded.
        unsafe {
            PageAllocator::instance().dealloc(self);
        }
    }
}

unsafe impl Send for PageAllocation {}
unsafe impl Sync for PageAllocation {}
