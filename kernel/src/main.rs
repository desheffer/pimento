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

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    arch::init();

    println!("Hello, world!");

    cpu::hang();
}
