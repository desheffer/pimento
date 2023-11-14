use core::arch::asm;
use core::arch::global_asm;

use crate::abi::LocalInterruptHandler;
use crate::task::InterruptMask;

#[no_mangle]
pub extern "C" fn vector_irq() {
    LocalInterruptHandler::instance().handle();
}

#[no_mangle]
pub extern "C" fn vector_invalid(code: u64, esr_el1: u64, far_el1: u64) -> ! {
    panic!(
        "invalid vector entry {:#03x} (ESR = {:#018x}, FAR = {:#018x})",
        code, esr_el1, far_el1
    );
}

pub fn hang() -> ! {
    // SAFETY: This is the end.
    unsafe {
        InterruptMask::instance().disable_interrupts();
        loop {
            asm!("wfe");
        }
    }
}

global_asm!(
    include_str!("entry.s"),
    TASK_REGS_SIZE = const 34 * 8,
);
