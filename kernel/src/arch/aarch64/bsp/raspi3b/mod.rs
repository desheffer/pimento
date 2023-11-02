use crate::cpu::{self, InterruptHandler};
use crate::io;
use crate::kernel_main;

pub mod bcm2837_interrupt;
pub mod bcm2837_serial;

#[no_mangle]
pub extern "C" fn kernel_init() -> ! {
    bcm2837_serial::init();
    let serial = bcm2837_serial::serial().unwrap();
    io::set_console(serial);

    // TODO: Remove this block that exists solely for interrupt testing.
    InterruptHandler::instance().enable(&bcm2837_interrupt::CNTPNSIRQ, || unsafe {
        crate::println!("tick");
        crate::arch::time::set_timer(core::time::Duration::from_secs(1));
    });
    unsafe {
        crate::arch::time::set_timer(core::time::Duration::from_secs(0));
        crate::arch::cpu::enable_interrupts();
    }

    kernel_main();

    cpu::hang();
}
