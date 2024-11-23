use core::arch::asm;
use core::ptr;

use crate::memory::{
    Attribute, BlockDescriptorBuilder, Page, PageDescriptorBuilder, PhysicalAddress, Table,
    TableDescriptorBuilder, TableManager, LEVEL_ROOT,
};

const VA_BITS: u64 = 48; // Virtual addresses have a 16-bit prefix and a 48-bit address
pub const VA_START: usize = !((1 << VA_BITS) - 1); // Kernel memory starting address

const MAIR_EL1_DEVICE: u64 = 0x00; // Device-nGnRnE memory (no gathering, no re-ordering, and no early write)
const MAIR_EL1_NORMAL: u64 = 0xFF; // Normal memory
const MAIR_EL1_NORMAL_NC: u64 = 0x44; // Normal memory, non-cacheable

const MAIR_EL1: u64 = (MAIR_EL1_DEVICE << (8 * Attribute::Device as u64))
    | (MAIR_EL1_NORMAL << (8 * Attribute::Normal as u64))
    | (MAIR_EL1_NORMAL_NC << (8 * Attribute::NormalNC as u64));

const TCR_EL1_T0SZ: u64 = 64 - VA_BITS; // Size offset for TTBR0_EL1 is 2.pow(VA_BITS)
const TCR_EL1_TG0_4KB: u64 = 0b00 << 14; // 4 KB granule size for TTBR0_EL1
const TCR_EL1_T1SZ: u64 = (64 - VA_BITS) << 16; // Size offset for TTBR1_EL1 is 2.pow(VA_BITS)
const TCR_EL1_TG1_4KB: u64 = 0b10 << 30; // 4 KB granule size for TTBR1_EL1
const TCR_EL1_IPS_40BIT: u64 = 0b010 << 32; // 40 bits, 1 TB physical address size
const TCR_EL1_AS16: u64 = 0b1 << 36; // 16-bit ASID

const TCR_EL1: u64 = TCR_EL1_T0SZ
    | TCR_EL1_TG0_4KB
    | TCR_EL1_T1SZ
    | TCR_EL1_TG1_4KB
    | TCR_EL1_IPS_40BIT
    | TCR_EL1_AS16;

const SCTLR_EL1_M: u64 = 0b1 << 0; // Enable MMU
const SCTLR_EL1_C: u64 = 0b1 << 2; // Enable stage 1 data cache
const SCTLR_EL1_I: u64 = 0b1 << 12; // Enable stage 1 instruction cache

const L3_TABLE_COUNT: usize = 3; // The number of level 3 translation tables
const TABLE_COUNT: usize = 3 + L3_TABLE_COUNT; // The number of translation tables
static mut TABLES: [Table; TABLE_COUNT] = [const { Table::new() }; TABLE_COUNT];

extern "C" {
    static __end: u8;
}

fn table_descriptor(table: &TableManager) -> Result<TableDescriptorBuilder, ()> {
    let physical_address = PhysicalAddress::<Table>::new((table.table() as usize) & !VA_START);
    TableDescriptorBuilder::new(physical_address)
}

fn block_descriptor(address: *const ()) -> Result<BlockDescriptorBuilder, ()> {
    let physical_address = PhysicalAddress::<u8>::new((address as usize) & !VA_START);
    BlockDescriptorBuilder::new(physical_address)
}

fn page_descriptor(address: *const ()) -> Result<PageDescriptorBuilder, ()> {
    let physical_address = PhysicalAddress::<Page>::new((address as usize) & !VA_START);
    PageDescriptorBuilder::new(physical_address)
}

/// Creates kernel translation tables. Returns the address of the root table.
unsafe fn create_tables() -> Result<*mut Table, ()> {
    let table_l0 = TableManager::new(&mut TABLES[0], LEVEL_ROOT, ptr::null());
    let table_l1 = TableManager::new(&mut TABLES[1], LEVEL_ROOT + 1, table_l0.row(0).start());
    let table_l2 = TableManager::new(&mut TABLES[2], LEVEL_ROOT + 2, table_l1.row(0).start());
    let table_l3 = [
        TableManager::new(&mut TABLES[3], LEVEL_ROOT + 3, table_l2.row(0).start()),
        TableManager::new(&mut TABLES[4], LEVEL_ROOT + 3, table_l2.row(1).start()),
        TableManager::new(&mut TABLES[5], LEVEL_ROOT + 3, table_l2.row(2).start()),
    ];

    // Level 0 [0, _, _, _]:
    let row = table_l0.row(0);
    row.write_table(&table_descriptor(&table_l1)?)?;

    // Level 1 [0, i, _, _]:
    for i in 0..table_l1.row_count() {
        let row = table_l1.row(i);
        if i == 0 {
            row.write_table(&table_descriptor(&table_l2)?)?;
        } else {
            let desc = block_descriptor(row.start())?.set_attribute(Attribute::Device);
            row.write_block(&desc)?;
        }
    }

    // Level 2 [0, 0, i, _]:
    for i in 0..table_l2.row_count() {
        let row = table_l2.row(i);

        // Level 3 [0, 0, i, j]:
        if i < L3_TABLE_COUNT {
            row.write_table(&table_descriptor(&table_l3[i])?)?;
            for j in 0..table_l3[i].row_count() {
                let row = table_l3[i].row(j);
                let desc = page_descriptor(row.start())?.set_attribute(Attribute::Normal);
                row.write_page(&desc)?;
            }
        } else {
            let desc = block_descriptor(row.start())?.set_attribute(Attribute::Device);
            row.write_block(&desc)?;
        }
    }

    Ok(table_l0.table())
}

/// Enables virtual memory.
///
/// This function is called early in the initialization process.
#[no_mangle]
unsafe extern "C" fn virtual_memory_early_init() {
    let table_l0 = create_tables().unwrap();

    // Set predefined memory attributes.
    asm!(
        "msr mair_el1, {mair_el1}",
        "msr tcr_el1, {tcr_el1}",
        "isb",
        mair_el1 = in(reg) MAIR_EL1,
        tcr_el1 = in(reg) TCR_EL1,
    );

    // Set the translation tables for user space (temporary) and kernel space. The ASID is omitted
    // because it is 0.
    asm!(
        "msr ttbr0_el1, {ttbr0_el1}",
        "msr ttbr1_el1, {ttbr1_el1}",
        "isb",
        ttbr0_el1 = in(reg) table_l0,
        ttbr1_el1 = in(reg) table_l0,
    );

    // Enable the MMU.
    asm!(
        "mrs {sctlr_el1}, sctlr_el1",
        "orr {sctlr_el1}, {sctlr_el1}, {SCTLR_EL1_C}",
        "orr {sctlr_el1}, {sctlr_el1}, {SCTLR_EL1_I}",
        "orr {sctlr_el1}, {sctlr_el1}, {SCTLR_EL1_M}",
        "msr sctlr_el1, {sctlr_el1}",
        "isb",
        sctlr_el1 = out(reg) _,
        SCTLR_EL1_C = const SCTLR_EL1_C,
        SCTLR_EL1_I = const SCTLR_EL1_I,
        SCTLR_EL1_M = const SCTLR_EL1_M,
    );
}
