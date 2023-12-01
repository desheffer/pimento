#![allow(dead_code)]
#![feature(asm_const)]
#![feature(coerce_unsized)]
#![feature(dispatch_from_dyn)]
#![feature(format_args_nl)]
#![feature(panic_info_message)]
#![feature(unsize)]
#![no_main]
#![no_std]

extern crate alloc;

mod abi;
mod boot;
mod context;
mod device;
mod memory;
mod panic;
mod print;
mod sync;

pub fn kernel_main() {
    println!("Hello, world!");

    // TODO: Remove when kernel is capable of starting other processes.
    #[allow(clippy::empty_loop)]
    loop {}
}
