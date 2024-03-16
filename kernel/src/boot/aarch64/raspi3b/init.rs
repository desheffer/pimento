use core::ops::Add;
use core::time::Duration;

use alloc::vec;

use crate::abi::{system_calls_table, InterruptRouter, SystemCallRouter, VectorTable};
use crate::context::{AArch64ContextSwitch, Scheduler, TaskCreationService};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{Bcm2837InterruptController, CNTPNSIRQ};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::device::Monotonic;
use crate::kernel_main;
use crate::memory::{PageAllocator, PhysicalAddress};
use crate::print;
use crate::sync::{Arc, OnceLock};

extern "C" {
    static __end: u8;
}

/// Stores the given expression using a static `OnceLock` and returns a static reference.
macro_rules! static_get_or_init {
    ($type:ty, $init:expr $(,)?) => {{
        static ONCE_LOCK: OnceLock<$type> = OnceLock::new();
        ONCE_LOCK.get_or_init(|| $init)
    }};
}

/// Initializes a Raspberry Pi 3 B.
///
/// This initialization sequence is designed so that, at some point in the future, it might be
/// loaded from a Device Tree Blob (DTB).
#[no_mangle]
pub unsafe extern "C" fn kernel_init() -> ! {
    let serial = Arc::new(Bcm2837Serial::new());
    serial.init();
    print::set_logger(serial.clone());

    let timer = Arc::new(ArmV8Timer::new());
    print::set_monotonic(timer.clone());

    let page_allocator = static_get_or_init!(
        PageAllocator,
        PageAllocator::new(
            0x4000_0000,
            vec![
                PhysicalAddress::new(0)..PhysicalAddress::from_ptr(&__end),
                PhysicalAddress::new(0x3F00_0000)..PhysicalAddress::new(0x4000_0000),
            ],
        )
    );

    let context_switcher = static_get_or_init!(AArch64ContextSwitch, AArch64ContextSwitch::new());

    let scheduler = static_get_or_init!(
        Scheduler,
        Scheduler::new(
            1,
            timer.clone(),
            Duration::from_millis(10),
            context_switcher
        )
    );

    let task_creation = TaskCreationService::new(scheduler, page_allocator);
    task_creation.create_and_become_kinit();

    let interrupt_router = static_get_or_init!(InterruptRouter, InterruptRouter::new());

    let interrupt_controller = Arc::new(Bcm2837InterruptController::new());
    interrupt_router.enable(
        interrupt_controller,
        CNTPNSIRQ,
        Scheduler::schedule,
        scheduler,
    );

    let system_call_router = static_get_or_init!(
        SystemCallRouter,
        SystemCallRouter::new(system_calls_table())
    );

    let vector_table = VectorTable::new(interrupt_router, system_call_router);
    vector_table.install();

    scheduler.schedule();

    // Create example threads:
    task_creation.create_kthread(
        |_| loop {
            crate::print!("1");
            sleep_naively(Duration::from_secs(1));
        },
        &(),
    );
    task_creation.create_kthread(
        |_| loop {
            crate::print!("2");
            sleep_naively(Duration::from_secs(2));
        },
        &(),
    );

    kernel_main();

    unimplemented!("shutdown");
}

/// Burns CPU cycles for the given Duration.
fn sleep_naively(duration: Duration) {
    unsafe {
        // TODO: Don't instantiate a new timer.
        let monotonic = ArmV8Timer::new();
        let target = monotonic.monotonic().add(duration);

        while monotonic.monotonic() < target {
            core::arch::asm!("wfi");
        }
    }
}
