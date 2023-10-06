#![feature(format_args_nl)]
#![feature(panic_info_message)]
#![no_main]
#![no_std]

mod arch;
mod console;
mod panic;
mod print;

#[no_mangle]
pub extern "C" fn kernel_main() -> ! {
    arch::init();

    println!("Hello, world!");

    arch::hang();
}
