use core::ops::Add;
use core::time::Duration;

use crate::abi::{hang, LocalInterruptHandler};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{
    Bcm2837Interrupt, Bcm2837InterruptController, CNTPNSIRQ,
};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::device::Registry;
use crate::kernel_main;
use crate::memory::PageAllocator;
use crate::sync::OnceLock;
use crate::task::{InterruptMask, Scheduler};

extern "C" {
    static mut __end: u8;
}

/// Initialize a Raspberry Pi 3 B
///
/// This initialization sequence is designed so that, at some point in the future, it might be
/// loaded from a Device Tree Blob (DTB).
#[no_mangle]
pub unsafe extern "C" fn kernel_init() -> ! {
    let timer = TIMER.get_or_init(|| ArmV8Timer::new());
    Registry::instance().set_timer(timer);
    Registry::instance().set_counter(timer);

    let serial = SERIAL.get_or_init(|| Bcm2837Serial::new());
    serial.init();
    Registry::instance().set_logger(serial);

    let page_allocator = PageAllocator::instance();
    let end = &mut __end as *mut u8;
    page_allocator.set_capacity(0x40000000);
    page_allocator.add_reserved_range(0..(end as usize));
    page_allocator.add_reserved_range(0x3F000000..0x40000000);

    let intr_controller = INTR_CONTROLLER.get_or_init(|| Bcm2837InterruptController::new());
    let cntpnsirq = CNTPNSIRQ_INTR.get_or_init(|| intr_controller.interrupt(CNTPNSIRQ));
    LocalInterruptHandler::instance().enable(cntpnsirq, || Scheduler::instance().schedule());

    let scheduler = Scheduler::instance();
    scheduler.set_num_cores(1);
    scheduler.set_after_schedule(|| {
        let timer = Registry::instance().timer().unwrap();
        timer.set_duration(core::time::Duration::from_millis(1));
        InterruptMask::instance().enable_interrupts();
    });
    scheduler.create_and_become_init();
    scheduler.schedule();

    // Create example threads:
    scheduler.create_kthread(|| loop {
        crate::println!("[kthread 1 - 1 sec]");
        let counter = Registry::instance().counter().unwrap();
        let target = counter.uptime().add(Duration::from_secs(1));
        while counter.uptime() < target {
            core::arch::asm!("nop");
        }
    });
    scheduler.create_kthread(|| loop {
        crate::println!("[kthread 2 - 2 sec]");
        let counter = Registry::instance().counter().unwrap();
        let target = counter.uptime().add(Duration::from_secs(2));
        while counter.uptime() < target {
            core::arch::asm!("nop");
        }
    });

    kernel_main();

    hang();
}

static TIMER: OnceLock<ArmV8Timer> = OnceLock::new();
static SERIAL: OnceLock<Bcm2837Serial> = OnceLock::new();
static INTR_CONTROLLER: OnceLock<Bcm2837InterruptController> = OnceLock::new();
static CNTPNSIRQ_INTR: OnceLock<Bcm2837Interrupt> = OnceLock::new();
