use core::arch::asm;

use alloc::vec::Vec;

use crate::memory::{PageAllocation, PageAllocator, PhysicalAddress, Table};
use crate::sync::{Arc, Mutex};

const TTBR_EL1_ASID_SHIFT: u64 = 48;

/// AArch64 auto-incrementing Address Space ID (ASID).
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub struct AddressSpaceId {
    id: u16,
}

impl AddressSpaceId {
    /// Generates the next available Address Space ID.
    pub fn next() -> Self {
        static NEXT: Mutex<u16> = Mutex::new(1);
        let mut next = NEXT.lock();
        let id = *next;
        *next += 1;
        Self { id }
    }
}

/// AArch64 translation table for a task.
pub struct MemoryContext {
    page_allocator: Option<&'static PageAllocator>,
    page_allocations: Vec<Arc<PageAllocation>>,
    asid: AddressSpaceId,
    table_l0: *mut Table,
}

impl MemoryContext {
    /// Creates a memory context for kernel virtual memory.
    ///
    /// This function is called early in the initialization process.
    pub fn new_for_kinit(table_l0: *mut Table) -> Self {
        let page_allocations = Vec::new();

        Self {
            page_allocator: None,
            page_allocations,
            asid: AddressSpaceId::next(),
            table_l0,
        }
    }

    /// Creates a memory context.
    pub fn new(page_allocator: &'static PageAllocator) -> Self {
        let mut page_allocations = Vec::new();

        // SAFETY: Safe because the page allocation is recorded.
        let table_l0: *mut Table;
        unsafe {
            let table_l0_page = Arc::new(page_allocator.alloc());
            table_l0 = table_l0_page.as_mut_ptr() as *mut _;
            page_allocations.push(table_l0_page);
        }

        Self {
            page_allocator: Some(page_allocator),
            page_allocations,
            asid: AddressSpaceId::next(),
            table_l0,
        }
    }

    /// Allocates a page and records the allocation.
    pub fn alloc_unmapped_page(&mut self) -> Arc<PageAllocation> {
        // SAFETY: Safe because the page allocation is recorded.
        unsafe {
            let allocation = Arc::new(self.page_allocator.unwrap().alloc());
            self.page_allocations.push(allocation.clone());
            allocation
        }
    }

    /// Generates the TTBR (Translation Table Base Register) value for this memory context.
    pub fn ttbr(&self) -> u64 {
        let asid = self.asid.id as u64;
        let table: u64 = PhysicalAddress::from_ptr(self.table_l0).into();
        (asid << TTBR_EL1_ASID_SHIFT) | table
    }
}

/// Performs the memory-specific steps of a context switch.
pub unsafe fn memory_context_switch(next: &mut MemoryContext) {
    // Set the translation table for user space.
    asm!(
        "msr ttbr0_el1, {ttbr0_el1}",
        "isb",
        ttbr0_el1 = in(reg) next.ttbr(),
    );
}
