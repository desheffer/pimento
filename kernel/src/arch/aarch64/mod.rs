use core::arch::asm;

mod bsp;
pub mod lock;
mod start;

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
