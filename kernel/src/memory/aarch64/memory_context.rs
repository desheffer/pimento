use core::arch::asm;
use core::cell::UnsafeCell;
use core::ptr;

use alloc::vec::Vec;

use crate::memory::{
    Attribute, DescriptorType, Page, PageAllocation, PageAllocator, PageDescriptorBuilder,
    PhysicalAddress, Table, TableDescriptorBuilder, TableManager, UserVirtualAddress, LEVEL_MAX,
    LEVEL_ROOT, MEMORY_MAPPER,
};
use crate::sync::{Arc, Lock, Mutex};

const TTBR_EL1_ASID_SHIFT: u64 = 48;

/// An AArch64 auto-incrementing Address Space ID (ASID).
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub struct AddressSpaceId {
    id: u16,
}

impl AddressSpaceId {
    /// Generates the next available Address Space ID.
    pub fn next() -> Self {
        static NEXT: Mutex<u16> = Mutex::new(0);
        let mut next = NEXT.lock();
        let id = *next;
        *next += 1;
        Self { id }
    }
}

/// An AArch64 translation table for a task.
pub struct MemoryContext {
    lock: Lock,
    page_allocator: Option<&'static PageAllocator>,
    page_allocations: UnsafeCell<Vec<Arc<PageAllocation>>>,
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
            lock: Lock::new(),
            page_allocator: None,
            page_allocations: UnsafeCell::new(page_allocations),
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
            table_l0 = table_l0_page.page() as _;
            page_allocations.push(table_l0_page);
        }

        Self {
            lock: Lock::new(),
            page_allocator: Some(page_allocator),
            page_allocations: UnsafeCell::new(page_allocations),
            asid: AddressSpaceId::next(),
            table_l0,
        }
    }

    /// Generates the TTBR (Translation Table Base Register) value for this memory context.
    pub fn ttbr(&self) -> u64 {
        let asid = self.asid.id as u64;
        let table = MEMORY_MAPPER.physical_address(self.table_l0).address();
        (asid << TTBR_EL1_ASID_SHIFT) | table as u64
    }

    /// Allocates a page and records the allocation.
    pub fn alloc_unmapped_page(&self) -> Arc<PageAllocation> {
        // SAFETY: Safe because the call is behind a lock.
        self.lock.call(|| unsafe {
            let page_allocations = &mut *self.page_allocations.get();
            let allocation = Arc::new(self.page_allocator.unwrap().alloc());
            page_allocations.push(allocation.clone());
            allocation
        })
    }

    /// Allocates a page, maps it to the given virtual address, and records the allocation.
    pub fn alloc_mapped_page<T>(&self, address: UserVirtualAddress<T>) {
        // SAFETY: Safe because the call is behind a lock.
        self.lock.call(|| unsafe {
            let page_allocations = &mut *self.page_allocations.get();

            let mut table = TableManager::new(self.table_l0, LEVEL_ROOT, ptr::null());

            // Iterate from level 0 to level 2. At each level, find or create a table to map the
            // given input address.
            while table.level() < LEVEL_MAX {
                let index = table.input_address_index(address.ptr() as _).unwrap();
                let row = table.row(index);

                let next_table_data = match row.descriptor_type() {
                    // If a table already exists, then load it.
                    Some(DescriptorType::Table) => row.load_table().address(),
                    // Otherwise, create a table.
                    _ => {
                        let allocation = Arc::new(self.page_allocator.unwrap().alloc());
                        let page: PhysicalAddress<Table> = allocation.address().convert();
                        page_allocations.push(allocation);

                        let builder = TableDescriptorBuilder::new_with_address(page);
                        row.write_table(builder);
                        page
                    }
                };

                table = TableManager::new(
                    MEMORY_MAPPER.virtual_address(next_table_data),
                    table.level() + 1,
                    row.input_address_start(),
                );
            }

            // At level 3, allocate and map the target page.
            {
                let index = table.input_address_index(address.ptr() as _).unwrap();
                let row = table.row(index);

                match row.descriptor_type() {
                    // If a page is already mapped, then do nothing.
                    Some(DescriptorType::Page) => {}
                    // Otherwise, create a page.
                    _ => {
                        let allocation = Arc::new(self.page_allocator.unwrap().alloc());
                        let page: PhysicalAddress<Page> = allocation.address().convert();
                        page_allocations.push(allocation);

                        let mut builder = PageDescriptorBuilder::new_with_address(page);
                        builder.set_attribute(Attribute::Normal);
                        row.write_page(builder);
                    }
                };
            }
        })
    }

    /// Copies data from a kernel virtual address to a user virtual address.
    pub unsafe fn copy_to_user<T>(&self, src: *const T, dst: UserVirtualAddress<T>, count: usize) {
        core::ptr::copy(src, dst.ptr(), count);
    }

    /// Copies data from a user virtual address to a kernel virtual address.
    pub unsafe fn copy_from_user<T>(&self, src: UserVirtualAddress<T>, dst: *mut T, count: usize) {
        core::ptr::copy(src.ptr(), dst, count);
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
