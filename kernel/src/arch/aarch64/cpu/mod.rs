use core::arch::asm;

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
