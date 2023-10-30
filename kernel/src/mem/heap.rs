use core::alloc::{GlobalAlloc, Layout};
use core::cell::UnsafeCell;
use core::ptr;

use crate::sync::Mutex;

/// A simple heap allocator
///
/// Allocations are made linearly. Deallocations are not implemented.
#[derive(Debug)]
struct Allocator<const N: usize> {
    heap: UnsafeCell<[u8; N]>,
    reserved: Mutex<usize>,
}

impl<const N: usize> Allocator<N> {
    const fn new() -> Self {
        Self {
            heap: UnsafeCell::new([0; N]),
            reserved: Mutex::new(0),
        }
    }
}

unsafe impl<const N: usize> GlobalAlloc for Allocator<N> {
    unsafe fn alloc(&self, layout: Layout) -> *mut u8 {
        let start = self.heap.get() as *mut u8;
        let mut reserved = self.reserved.lock();

        // Reserve space to accommodate the requested alignment.
        let align_offset = start.add(*reserved).align_offset(layout.align());
        *reserved += align_offset;

        // Record the starting point of this allocation.
        let alloc_start = start.add(*reserved);

        // Reserve space to accommodate the requested size.
        *reserved += layout.size();

        // Return a null pointer if the heap is exhausted.
        if *reserved > N {
            return ptr::null_mut();
        }

        alloc_start
    }

    unsafe fn dealloc(&self, _ptr: *mut u8, _layout: Layout) {}
}

#[global_allocator]
static ALLOCATOR: Allocator<{ 4 * 1024 * 1024 }> = Allocator::new();

unsafe impl<const N: usize> Sync for Allocator<N> {}
