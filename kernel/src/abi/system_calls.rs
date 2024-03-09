use crate::abi::{SystemCallRouter, SYS};

/// Registers functions to handle each system call.
pub fn register_system_calls() {
    SystemCallRouter::register(SYS::WRITE, |_, _, _, _, _, _| {
        crate::print!("\n[WRITE]\n");
        0
    });
}
