use crate::arch;
use crate::println;
use core::panic::PanicInfo;

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
    if let Some(message) = info.message() {
        println!("{}", message);
    }

    arch::hang();
}
