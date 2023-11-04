use crate::device::bcm2837_interrupt::CNTPNSIRQ;
use crate::device::bcm2837_serial::BCM2837Serial;
use crate::interrupt::{enable_interrupts, hang, LocalInterruptHandler};
use crate::io::set_console;
use crate::kernel_main;
use crate::time::Timer;

#[no_mangle]
pub unsafe extern "C" fn kernel_init() -> ! {
    set_console(BCM2837Serial::instance());

    LocalInterruptHandler::instance().enable(&CNTPNSIRQ, || {
        crate::println!("tick");
        Timer::instance().set_timer(core::time::Duration::from_secs(1));
    });
    Timer::instance().set_timer(core::time::Duration::from_secs(0));
    enable_interrupts();

    kernel_main();

    hang();
}
