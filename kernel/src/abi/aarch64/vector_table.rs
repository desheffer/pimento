use core::arch::{asm, global_asm};
use core::mem::size_of;
use core::ptr::addr_of;

use crate::abi::{InterruptRouter, SystemCallRouter};
use crate::sync::OnceLock;

const ESR_EL1_EC_SHIFT: u64 = 26;
const ESR_EL1_EC_MASK: u64 = 0b111111 << ESR_EL1_EC_SHIFT;

const ESR_EL1_EC_SVC64: u64 = 0b010101; // SVC instruction execution in AArch64 state

extern "C" {
    static mut vector_table: u8;
}

/// A vector table manager.
pub struct VectorTable {
    local_interrupt_handler: &'static InterruptRouter,
    system_call_handler: &'static SystemCallRouter,
}

impl VectorTable {
    /// Creates a vector table.
    pub fn new(
        local_interrupt_handler: &'static InterruptRouter,
        system_call_handler: &'static SystemCallRouter,
    ) -> Self {
        Self {
            local_interrupt_handler,
            system_call_handler,
        }
    }

    /// Installs this vector table as the system vector table.
    pub unsafe fn install(&'static self) {
        INSTALLED_TABLE
            .set(self)
            .unwrap_or_else(|_| panic!("installing vector table failed"));

        asm!(
            "msr vbar_el1, {}",
            "isb",
            in(reg) addr_of!(vector_table),
        );
    }
}

static INSTALLED_TABLE: OnceLock<&'static VectorTable> = OnceLock::new();

/// The AArch64 registers to save when entering the vector table.
#[repr(C)]
pub struct Registers {
    x0: u64,
    x1: u64,
    x2: u64,
    x3: u64,
    x4: u64,
    x5: u64,
    x6: u64,
    x7: u64,
    x8: u64,
    x9: u64,
    x10: u64,
    x11: u64,
    x12: u64,
    x13: u64,
    x14: u64,
    x15: u64,
    x16: u64,
    x17: u64,
    x18: u64,
    x19: u64,
    x20: u64,
    x21: u64,
    x22: u64,
    x23: u64,
    x24: u64,
    x25: u64,
    x26: u64,
    x27: u64,
    x28: u64,
    fp: u64, // x29
    lr: u64, // x30
    sp_el0: u64,
    elr_el1: u64,
    spsr_el1: u64,
}

/// Handles synchronous kernel exceptions from the vector table.
#[no_mangle]
pub unsafe extern "C" fn vector_sync_el1(
    _regs: *mut Registers,
    esr_el1: u64,
    far_el1: u64,
    elr_el1: u64,
) {
    panic!(
        "synchronous exception in EL1 (ESR = {:#018x}, ELR = {:#018x}, FAR = {:#018x})",
        esr_el1, elr_el1, far_el1
    );
}

/// Handles synchronous user exceptions from the vector table.
#[no_mangle]
pub unsafe extern "C" fn vector_sync_el0(
    regs: *mut Registers,
    esr_el1: u64,
    far_el1: u64,
    elr_el1: u64,
) {
    match (esr_el1 & ESR_EL1_EC_MASK) >> ESR_EL1_EC_SHIFT {
        ESR_EL1_EC_SVC64 => {
            let table = INSTALLED_TABLE.get().unwrap();
            (*regs).x0 = table.system_call_handler.handle(
                (*regs).x8 as _,
                (*regs).x0 as _,
                (*regs).x1 as _,
                (*regs).x2 as _,
                (*regs).x3 as _,
                (*regs).x4 as _,
                (*regs).x5 as _,
            ) as _;
        }
        _ => {
            panic!(
                "synchronous exception in EL0 (ESR = {:#018x}, ELR = {:#018x}, FAR = {:#018x})",
                esr_el1, elr_el1, far_el1
            );
        }
    }
}

/// Handles IRQ exceptions from the vector table.
#[no_mangle]
pub unsafe extern "C" fn vector_irq() {
    let table = INSTALLED_TABLE.get().unwrap();
    table.local_interrupt_handler.handle();
}

/// Handles invalid entries from the vector table.
#[no_mangle]
pub extern "C" fn vector_invalid(code: u64, esr_el1: u64, far_el1: u64) -> ! {
    panic!(
        "invalid vector entry {:#03x} (ESR = {:#018x}, FAR = {:#018x})",
        code, esr_el1, far_el1
    );
}

global_asm!(
    include_str!("vector_table.s"),
    TASK_REGS_SIZE = const size_of::<Registers>(),
);
