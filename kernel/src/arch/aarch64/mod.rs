mod bsp;
mod start;

use core::arch::asm;

pub fn init() {
    bsp::init();
}

pub fn hang() -> ! {
    loop {
        unsafe {
            asm!("wfe");
        }
    }
}

pub fn nop() {
    unsafe {
        asm!("nop");
    }
}
