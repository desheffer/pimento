use core::ops::Add;
use core::time::Duration;

use alloc::vec;

use crate::abi::{hang, Entry, LocalInterruptHandler};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{Bcm2837InterruptController, CNTPNSIRQ};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::device::Counter;
use crate::kernel_main;
use crate::memory::PageAllocator;
use crate::print::PrintRegistry;
use crate::sync::{Arc, OnceLock};
use crate::task::{Scheduler, _scheduler_schedule};

const QUANTUM: Duration = Duration::from_millis(10);

extern "C" {
    static mut __end: u8;
}

/// Initialize a Raspberry Pi 3 B
///
/// This initialization sequence is designed so that, at some point in the future, it might be
/// loaded from a Device Tree Blob (DTB).
#[no_mangle]
pub unsafe extern "C" fn kernel_init() -> ! {
    let timer = Arc::new(ArmV8Timer::new());
    let serial = Arc::new(Bcm2837Serial::new());
    serial.init();
    PrintRegistry::set_counter(timer.clone());
    PrintRegistry::set_logger(serial.clone());

    let end = &mut __end as *mut u8;
    let page_allocator = Arc::new(PageAllocator::new(
        0x40000000,
        vec![0..(end as usize), 0x3F000000..0x40000000],
    ));

    let local_interrupt_handler = Arc::new(LocalInterruptHandler::new());

    let entry = Entry::new(local_interrupt_handler.clone());
    entry.install();

    let scheduler = Arc::new(Scheduler::new(
        1,
        QUANTUM,
        timer.clone(),
        page_allocator.clone(),
    ));
    scheduler.create_and_become_init();

    let interrupt_controller = Arc::new(Bcm2837InterruptController::new());
    local_interrupt_handler.enable(
        interrupt_controller,
        CNTPNSIRQ,
        _scheduler_schedule,
        &scheduler as *const Arc<Scheduler> as *const (),
    );

    scheduler.schedule();

    // Create example threads:
    static COUNTER: OnceLock<Arc<dyn Counter>> = OnceLock::new();
    COUNTER.set(timer.clone()).unwrap();
    scheduler.create_kthread(|| loop {
        let counter = COUNTER.get().unwrap();
        let target = counter.uptime().add(Duration::from_secs(1));
        while counter.uptime() < target {
            core::arch::asm!("wfi");
        }
        crate::print!("1");
    });
    scheduler.create_kthread(|| loop {
        let counter = COUNTER.get().unwrap();
        let target = counter.uptime().add(Duration::from_secs(2));
        while counter.uptime() < target {
            core::arch::asm!("wfi");
        }
        crate::print!("2");
    });

    kernel_main();

    hang();
}
