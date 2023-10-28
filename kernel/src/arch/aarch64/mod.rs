use core::arch::asm;

mod bsp;
pub mod lock;
mod start;

pub fn init() {
    bsp::init();
}

pub fn hang() -> ! {
    loop {
        // SAFETY: Does not affect other threads.
        unsafe {
            asm!("wfe");
        }
    }
}

pub fn nop() {
    // SAFETY: Does not affect other threads.
    unsafe {
        asm!("nop");
    }
}
