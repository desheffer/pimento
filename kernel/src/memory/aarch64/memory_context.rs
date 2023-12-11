use core::arch::asm;

use alloc::vec::Vec;

use crate::memory::{PageAllocation, PageAllocator, PhysicalAddress, Table};
use crate::sync::{Arc, Mutex};

const TTBR_EL1_ASID_SHIFT: u64 = 48;
const TABLE_ADDR_MASK: u64 = 0x0000_FFFF_FFFF_F000;

/// AArch64 auto-incrementing Address Space ID (ASID).
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub struct AddressSpaceId {
    id: u16,
}

impl AddressSpaceId {
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
    page_allocations: Vec<Arc<PageAllocation>>,
    asid: AddressSpaceId,
    table_l0: *mut Table,
}

impl MemoryContext {
    pub unsafe fn new_for_kinit(table_l0: *mut Table) -> Self {
        Self {
            page_allocations: Vec::new(),
            asid: AddressSpaceId::next(),
            table_l0,
        }
    }

    pub unsafe fn new() -> Self {
        let mut page_allocations = Vec::new();

        let table_l0_page = Arc::new(PageAllocator::instance().alloc());
        let table_l0 = table_l0_page.address().as_mut_ptr() as *mut Table;
        page_allocations.push(table_l0_page);

        Self {
            page_allocations,
            asid: AddressSpaceId::next(),
            table_l0,
        }
    }

    /// Allocates a page and records the allocation.
    pub unsafe fn alloc_unmapped_page(&mut self) -> Arc<PageAllocation> {
        let allocation = Arc::new(PageAllocator::instance().alloc());
        self.page_allocations.push(allocation.clone());
        allocation
    }

    pub unsafe fn ttbr(&self) -> u64 {
        let asid = self.asid.id as u64;
        let table: u64 = PhysicalAddress::from_ptr(self.table_l0).into();
        (asid << TTBR_EL1_ASID_SHIFT) | table
    }
}

pub unsafe fn memory_context_switch(next: &mut MemoryContext) {
    // Set the translation table for user space.
    asm!(
        "msr ttbr0_el1, {ttbr0_el1}",
        "isb",
        ttbr0_el1 = in(reg) next.ttbr(),
    );
}
