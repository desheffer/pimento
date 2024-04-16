#![no_main]
#![no_std]

use core::arch::global_asm;
use core::panic::PanicInfo;

global_asm!(include_str!("main.s"));

#[panic_handler]
fn panic(_info: &PanicInfo) -> ! {
    loop {}
}
