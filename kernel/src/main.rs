#![feature(format_args_nl)]
#![feature(panic_info_message)]
#![no_main]
#![no_std]

extern crate alloc;

mod arch;
mod cell;
mod console;
mod heap;
mod mutex;
mod panic;
mod print;

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    arch::init();

    println!("Hello, world!");

    arch::hang();
}
