use crate::device::bcm2837_interrupt::{BCM2837Interrupt, BCM2837InterruptController, CNTPNSIRQ};
use crate::device::bcm2837_serial::BCM2837Serial;
use crate::interrupt::{enable_interrupts, hang, LocalInterruptHandler};
use crate::io::set_console;
use crate::kernel_main;
use crate::sync::OnceLock;
use crate::time::Timer;

/// Initialize a Raspberry Pi 3 B
///
/// This initialization sequence is designed so that, at some point in the future, it might be
/// loaded from a Device Tree Blob (DTB).
#[no_mangle]
pub unsafe extern "C" fn kernel_init() -> ! {
    let serial = SERIAL.get_or_init(|| BCM2837Serial::new());
    serial.init();
    set_console(serial);

    let intr_controller = INTR_CONTROLLER.get_or_init(|| BCM2837InterruptController::new());
    let cntpnsirq = CNTPNSIRQ_INTR.get_or_init(|| intr_controller.interrupt(CNTPNSIRQ));
    LocalInterruptHandler::instance().enable(cntpnsirq, || {
        crate::println!("tick");
        Timer::instance().set_timer(core::time::Duration::from_secs(1));
    });

    Timer::instance().set_timer(core::time::Duration::from_secs(0));
    enable_interrupts();

    kernel_main();

    hang();
}

static SERIAL: OnceLock<BCM2837Serial> = OnceLock::new();
static INTR_CONTROLLER: OnceLock<BCM2837InterruptController> = OnceLock::new();
static CNTPNSIRQ_INTR: OnceLock<BCM2837Interrupt> = OnceLock::new();
