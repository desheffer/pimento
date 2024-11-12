use core::arch::asm;
use core::panic::PanicInfo;

use crate::println;

#[panic_handler]
fn panic(info: &PanicInfo) -> ! {
    if let Some(location) = info.location() {
        println!(
            "kernel panicked at {}:{}:{}:",
            location.file(),
            location.line(),
            location.column()
        );
    }
    println!("{}", info.message());

    hang();
}

/// Halts the current core and enters a low-power state.
#[cfg(target_arch = "aarch64")]
fn hang() -> ! {
    // SAFETY: This is the point of no return.
    unsafe {
        asm!("msr daifset, #0b1111");
        loop {
            asm!("wfe");
        }
    }
}
