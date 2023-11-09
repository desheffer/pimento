#![allow(dead_code)]
#![feature(asm_const)]
#![feature(core_intrinsics)]
#![feature(format_args_nl)]
#![feature(panic_info_message)]
#![no_main]
#![no_std]

extern crate alloc;

mod boot;
mod device;
mod interrupt;
mod io;
mod memory;
mod sync;
mod task;
mod time;

pub fn kernel_main() {
    println!("Hello, world!");

    // TODO: Remove when kernel is capable of starting other processes.
    loop {}
}
