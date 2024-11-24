use core::arch::asm;

/// Enables interrupts.
pub unsafe fn enable_interrupts() {
    asm!("msr daifclr, #0b0011");
}

/// Disables interrupts.
pub unsafe fn disable_interrupts() -> u64 {
    let daif: u64;

    // Save previous interrupts state.
    asm!("mrs {}, daif", out(reg) daif);

    // Disable interrupts.
    asm!("msr daifset, #0b0011");

    daif
}

/// Restores previous interrupt state.
unsafe fn restore_interrupts(daif: u64) {
    asm!("msr daif, {}", in(reg) daif);
}

/// Executes the given closure while interrupts are disabled.
pub fn critical<F, R>(f: F) -> R
where
    F: FnOnce() -> R,
{
    // SAFETY: Safe because the interrupt state is restored.
    unsafe {
        let daif = disable_interrupts();

        let result = f();

        restore_interrupts(daif);
        result
    }
}
