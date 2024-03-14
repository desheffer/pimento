use alloc::vec;
use alloc::vec::Vec;

use crate::abi::Sys;

use super::system_call_router::HandlerFn;

/// Generates a vector of system calls indexed by system call number.
pub fn system_calls_table() -> Vec<Option<HandlerFn>> {
    let mut table: Vec<Option<HandlerFn>> = vec![None; 256];

    table[Sys::Write as usize] = Some(|_, _, _, _, _, _| {
        crate::print!("\n[WRITE]\n");
        0
    });

    table
}
