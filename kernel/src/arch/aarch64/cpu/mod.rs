use core::arch::asm;

pub unsafe fn enable_interrupts() {
    asm!("msr daifclr, #2");
}

pub unsafe fn disable_interrupts() {
    asm!("msr daifset, #2");
}

pub fn hang() -> ! {
    // SAFETY: This is the end.
    unsafe {
        disable_interrupts();
        loop {
            asm!("wfe");
        }
    }
}
