#![allow(dead_code)]
#![feature(asm_const)]
#![feature(core_intrinsics)]
#![feature(format_args_nl)]
#![feature(panic_info_message)]
#![no_main]
#![no_std]

extern crate alloc;

mod abi;
mod boot;
mod device;
mod memory;
mod panic;
mod print;
mod sync;
mod task;

pub fn kernel_main() {
    println!("Hello, world!");

    // TODO: Remove when kernel is capable of starting other processes.
    loop {}
}
