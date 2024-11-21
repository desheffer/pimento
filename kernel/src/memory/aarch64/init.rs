use core::arch::asm;
use core::ptr::{self, addr_of, addr_of_mut};

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

static mut INIT_TABLE_L0: Table = Table::new();
static mut INIT_TABLE_L1_0: Table = Table::new();
static mut INIT_TABLE_L2_0_0: Table = Table::new();
static mut INIT_TABLE_L3_0_0_0: Table = Table::new();

extern "C" {
    static __end: u8;
}

/// Enable virtual memory.
///
/// This function is called early in the initialization process.
#[no_mangle]
unsafe extern "C" fn virtual_memory_early_init() {
    // Set predefined memory attributes.
    asm!(
        "msr mair_el1, {mair_el1}",
        "msr tcr_el1, {tcr_el1}",
        "isb",
        mair_el1 = in(reg) MAIR_EL1,
        tcr_el1 = in(reg) TCR_EL1,
    );

    let table_l0_data = addr_of_mut!(INIT_TABLE_L0);
    let table_l1_0_data = addr_of_mut!(INIT_TABLE_L1_0);
    let table_l2_0_0_data = addr_of_mut!(INIT_TABLE_L2_0_0);
    let table_l3_0_0_0_data = addr_of_mut!(INIT_TABLE_L3_0_0_0);

    let table_l0 = TableManager::new(table_l0_data, LEVEL_ROOT, ptr::null());
    let table_l1_0 = TableManager::new(
        table_l1_0_data,
        LEVEL_ROOT + 1,
        table_l0.row(0).input_address_start(),
    );
    let table_l2_0_0 = TableManager::new(
        table_l2_0_0_data,
        LEVEL_ROOT + 2,
        table_l1_0.row(0).input_address_start(),
    );
    let table_l3_0_0_0 = TableManager::new(
        table_l3_0_0_0_data,
        LEVEL_ROOT + 3,
        table_l2_0_0.row(0).input_address_start(),
    );

    // Level 0 [0]:
    let row = table_l0.row(0);
    let addr = PhysicalAddress::<Table>::new((table_l1_0_data as usize) & !VA_START);
    let builder = TableDescriptorBuilder::new_with_address(addr).unwrap();
    row.write_table(builder).unwrap();

    // Level 1 [0, 0]:
    let row = table_l1_0.row(0);
    let addr = PhysicalAddress::<Table>::new((table_l2_0_0_data as usize) & !VA_START);
    let builder = TableDescriptorBuilder::new_with_address(addr).unwrap();
    row.write_table(builder).unwrap();

    // Level 1 [0, N>=1]:
    for i in 1..table_l1_0.len() {
        let row = table_l1_0.row(i);
        let addr = PhysicalAddress::<u8>::new((row.input_address_start() as usize) & !VA_START);
        let mut builder = BlockDescriptorBuilder::new_with_address(addr).unwrap();
        builder.set_attribute(Attribute::Device);
        row.write_block(builder).unwrap();
    }

    // Level 2 [0, 0, 0]:
    let row = table_l2_0_0.row(0);
    let addr = PhysicalAddress::<Table>::new((table_l3_0_0_0_data as usize) & !VA_START);
    let builder = TableDescriptorBuilder::new_with_address(addr).unwrap();
    row.write_table(builder).unwrap();

    // Level 2 [0, 0, N>=1]:
    for i in 1..table_l2_0_0.len() {
        let row = table_l2_0_0.row(i);
        let addr = PhysicalAddress::<u8>::new((row.input_address_start() as usize) & !VA_START);
        let mut builder = BlockDescriptorBuilder::new_with_address(addr).unwrap();
        builder.set_attribute(Attribute::Device);
        row.write_block(builder).unwrap();
    }

    // Level 3 [0, 0, 0, N>=0]:
    for i in 0..table_l3_0_0_0.len() {
        let row = table_l3_0_0_0.row(i);
        let addr = PhysicalAddress::<Page>::new((row.input_address_start() as usize) & !VA_START);
        let mut builder = PageDescriptorBuilder::new_with_address(addr).unwrap();
        if addr.address() < addr_of!(__end) as _ {
            builder.set_attribute(Attribute::Normal);
        } else {
            builder.set_attribute(Attribute::Device);
        }
        row.write_page(builder).unwrap();
    }

    // Set the translation tables for user space (temporary) and kernel space. The ASID is omitted
    // because it is 0.
    asm!(
        "msr ttbr0_el1, {ttbr0_el1}",
        "msr ttbr1_el1, {ttbr1_el1}",
        "isb",
        ttbr0_el1 = in(reg) table_l0_data,
        ttbr1_el1 = in(reg) table_l0_data,
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
