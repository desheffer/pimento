#![feature(format_args_nl)]
#![feature(panic_info_message)]
#![no_main]
#![no_std]

extern crate alloc;

mod arch;
mod cpu;
mod io;
mod mem;
mod panic;
mod sync;

pub fn kernel_main() {
    println!("Hello, world!");
}
