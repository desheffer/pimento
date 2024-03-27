use core::arch::asm;
use core::ptr::{self, addr_of_mut};

use crate::memory::{
    Attribute, BlockDescriptorBuilder, MemoryContext, PhysicalAddress, Table,
    TableDescriptorBuilder, TableManager, LEVEL_ROOT,
};
use crate::sync::OnceLock;

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

const SCTLR_EL1_M: u64 = 0b1; // Enable MMU

static mut INIT_TABLE_L0: Table = Table::new();
static mut INIT_TABLE_L1: Table = Table::new();
static INIT_MEMORY_CONTEXT: OnceLock<MemoryContext> = OnceLock::new();

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
    let table_l1_data = addr_of_mut!(INIT_TABLE_L1);

    let table_l0 = TableManager::new(table_l0_data, LEVEL_ROOT, ptr::null());

    // Initialize level 0 as an identity map.
    let row = table_l0.row(0);
    let addr = PhysicalAddress::<Table>::new((table_l1_data as usize) & !VA_START);
    let builder = TableDescriptorBuilder::new_with_address(addr).unwrap();
    row.write_table(builder).unwrap();

    let table_l1 = TableManager::new(table_l1_data, LEVEL_ROOT + 1, row.input_address_start());

    // Initialize level 1 as an identity map.
    for i in 0..table_l1.len() {
        let row = table_l1.row(i);
        let addr = PhysicalAddress::<u8>::new((row.input_address_start() as usize) & !VA_START);
        let mut builder = BlockDescriptorBuilder::new_with_address(addr).unwrap();
        builder.set_attribute(Attribute::Device);
        row.write_block(builder).unwrap();
    }

    let memory_context = MemoryContext::new_for_kinit(addr_of_mut!(INIT_TABLE_L0));
    let ttbr = memory_context.ttbr();

    // Set the translation tables for user space (temporary) and kernel space.
    asm!(
        "msr ttbr0_el1, {ttbr0_el1}",
        "msr ttbr1_el1, {ttbr1_el1}",
        "isb",
        ttbr0_el1 = in(reg) ttbr,
        ttbr1_el1 = in(reg) ttbr,
    );

    // Enable the MMU.
    asm!(
        "mrs {sctlr_el1}, sctlr_el1",
        "orr {sctlr_el1}, {sctlr_el1}, {SCTLR_EL1_M}",
        "msr sctlr_el1, {sctlr_el1}",
        "isb",
        sctlr_el1 = out(reg) _,
        SCTLR_EL1_M = const SCTLR_EL1_M,
    );

    INIT_MEMORY_CONTEXT
        .set(memory_context)
        .unwrap_or_else(|_| panic!("setting initial memory context failed"));

    // TODO: Implement phase 2 that maps each section of kernel memory using the proper write and
    // execute permission bits.
}
