use core::arch::global_asm;

use crate::cpu::InterruptHandler;

#[no_mangle]
pub extern "C" fn vector_irq() {
    InterruptHandler::instance().handle();
}

#[no_mangle]
pub extern "C" fn vector_invalid(code: u64, esr_el1: u64, far_el1: u64) -> ! {
    panic!(
        "invalid vector entry {:#03x} (ESR = {:#018x}, FAR = {:#018x})",
        code, esr_el1, far_el1
    );
}

global_asm!(
    include_str!("entry.s"),
    PROCESS_REGS_SIZE = const 34 * 8,
);
