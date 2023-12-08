use crate::memory::PhysicalAddress;
use crate::sync::Mutex;

use super::page_table::Table;

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
    asid: AddressSpaceId,
    table_l0: *mut Table,
}

impl MemoryContext {
    pub fn new(asid: AddressSpaceId, table_l0: *mut Table) -> Self {
        Self { asid, table_l0 }
    }

    pub unsafe fn ttbr(&self) -> u64 {
        let asid = self.asid.id as u64;
        let table: u64 = PhysicalAddress::from_ptr(self.table_l0).into();
        (asid << TTBR_EL1_ASID_SHIFT) | table
    }
}
