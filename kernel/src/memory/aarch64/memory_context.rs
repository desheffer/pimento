use core::arch::asm;
use core::cell::UnsafeCell;
use core::ptr;
use core::sync::atomic::AtomicU16;
use core::sync::atomic::Ordering::Relaxed;

use alloc::vec::Vec;

use crate::memory::{
    Attribute, DescriptorType, Page, PageAllocation, PageAllocator, PageDescriptorBuilder,
    PhysicalAddress, Table, TableDescriptorBuilder, TableManager, UserAddress, LEVEL_MAX,
    LEVEL_ROOT, MEMORY_MAPPER,
};
use crate::sync::{Arc, Lock};

const TTBR_EL1_ASID_SHIFT: u64 = 48;

/// An AArch64 auto-incrementing Address Space ID (ASID).
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub struct AddressSpaceId {
    id: u16,
}

impl AddressSpaceId {
    /// Generates the next available Address Space ID.
    pub fn next() -> Self {
        static COUNTER: AtomicU16 = AtomicU16::new(1);
        Self {
            id: COUNTER.fetch_add(1, Relaxed),
        }
    }
}

/// The AArch64 translation table and pages allocated for a task.
pub struct MemoryContext {
    lock: Lock,
    page_allocator: &'static PageAllocator,
    page_allocations: UnsafeCell<Vec<Arc<PageAllocation>>>,
    asid: AddressSpaceId,
    table_l0: *mut Table,
}

impl MemoryContext {
    /// Creates a memory context.
    pub fn new(page_allocator: &'static PageAllocator) -> Result<Self, ()> {
        let mut page_allocations = Vec::new();

        // SAFETY: Safe because the page allocation is recorded.
        let table_l0: *mut Table;
        unsafe {
            let table_l0_page = Arc::new(page_allocator.alloc()?);
            table_l0 = table_l0_page.page() as _;
            page_allocations.push(table_l0_page);
        }

        Ok(Self {
            lock: Lock::new(),
            page_allocator,
            page_allocations: UnsafeCell::new(page_allocations),
            asid: AddressSpaceId::next(),
            table_l0,
        })
    }

    /// Generates the TTBR (Translation Table Base Register) value for this memory context.
    pub fn ttbr(&self) -> u64 {
        let asid = self.asid.id as u64;
        let table = MEMORY_MAPPER.physical_address(self.table_l0).address();
        (asid << TTBR_EL1_ASID_SHIFT) | table as u64
    }

    /// Allocates a page and records the allocation.
    ///
    /// The caller is responsible for ensuring that the lock is held during this process.
    unsafe fn alloc_page_raw(&self) -> Result<Arc<PageAllocation>, ()> {
        let page_allocations = &mut *self.page_allocations.get();

        let allocation = Arc::new(self.page_allocator.alloc()?);
        page_allocations.push(allocation.clone());

        Ok(allocation)
    }

    /// Allocates a page, but does not map it to a virtual address within this context.
    pub fn alloc_page_unmapped(&self) -> Result<Arc<PageAllocation>, ()> {
        // SAFETY: Safe because the call is behind a lock.
        self.lock.call(|| unsafe { self.alloc_page_raw() })
    }

    /// Creates or loads a table as a row within the given table manager.
    ///
    /// The caller is responsible for ensuring that the lock is held during this process.
    unsafe fn traverse_table_to_table<T>(
        &self,
        table: &TableManager,
        address: UserAddress<T>,
    ) -> Result<TableManager, ()> {
        let row = table.row_by_address(address.as_ptr() as _)?;

        let next_table = match row.descriptor_type() {
            Some(DescriptorType::Table) => row.load_table()?.address(),
            _ => {
                let allocation = self.alloc_page_raw()?;
                let page: PhysicalAddress<Table> = allocation.address().convert();

                let table_desc = TableDescriptorBuilder::new(page)?;
                row.write_table(&table_desc)?;

                page
            }
        };

        Ok(TableManager::new(
            MEMORY_MAPPER.virtual_address(next_table),
            table.level() + 1,
            row.start(),
        ))
    }

    /// Creates or loads a page as a row within the given table manager.
    ///
    /// The caller is responsible for ensuring that the lock is held during this process.
    unsafe fn traverse_table_to_page<T>(
        &self,
        table: &TableManager,
        address: UserAddress<T>,
    ) -> Result<PhysicalAddress<Page>, ()> {
        let row = table.row_by_address(address.as_ptr() as _)?;

        let page = match row.descriptor_type() {
            Some(DescriptorType::Page) => row.load_page()?.address(),
            _ => {
                let allocation = self.alloc_page_raw()?;
                let page: PhysicalAddress<Page> = allocation.address().convert();

                let page_desc = PageDescriptorBuilder::new(page)?
                    .set_attribute(Attribute::Normal)
                    .set_unprivileged(true);
                row.write_page(&page_desc)?;

                page
            }
        };

        Ok(page)
    }

    /// Allocates a page and maps it to the given virtual address within this context.
    pub fn alloc_page<T>(&self, address: UserAddress<T>) -> Result<PhysicalAddress<Page>, ()> {
        // SAFETY: Safe because the call is behind a lock.
        self.lock.call(|| unsafe {
            let mut table = TableManager::new(self.table_l0, LEVEL_ROOT, ptr::null());

            while table.level() < LEVEL_MAX {
                table = self.traverse_table_to_table(&table, address)?;
            }

            self.traverse_table_to_page(&table, address)
        })
    }

    /// Copies data from a kernel virtual address to a user virtual address.
    // TODO: Handle case where user virtual address is not mapped and/or would generate fault.
    pub unsafe fn copy_to_user<T>(
        &self,
        src: *const T,
        dst: UserAddress<T>,
        count: usize,
    ) -> Result<(), ()> {
        core::ptr::copy(src, dst.as_ptr(), count);

        Ok(())
    }

    /// Copies data from a user virtual address to a kernel virtual address.
    // TODO: Handle case where user virtual address is not mapped and/or would generate fault.
    pub unsafe fn copy_from_user<T>(
        &self,
        src: UserAddress<T>,
        dst: *mut T,
        count: usize,
    ) -> Result<(), ()> {
        core::ptr::copy(src.as_ptr(), dst, count);

        Ok(())
    }
}

/// Performs the memory-specific steps of a context switch.
pub unsafe fn memory_context_switch(next: &MemoryContext) {
    // Set the translation table for user space.
    asm!(
        "msr ttbr0_el1, {ttbr0_el1}",
        "isb",
        ttbr0_el1 = in(reg) next.ttbr(),
    );
}
