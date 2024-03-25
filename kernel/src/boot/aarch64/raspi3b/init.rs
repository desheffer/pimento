use core::ops::Add;
use core::time::Duration;

use alloc::borrow::ToOwned;
use alloc::vec;
use alloc::vec::Vec;

use crate::abi::system_calls::SysWrite;
use crate::abi::{InterruptRouter, SystemCall, SystemCallRouter, VectorTable};
use crate::context::{ContextSwitch, Scheduler, TaskCreationService, TaskExecutionService};
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

    let context_switcher = static_get_or_init!(ContextSwitch, ContextSwitch::new());

    let scheduler = static_get_or_init!(
        Scheduler,
        Scheduler::new(
            1,
            timer.clone(),
            Duration::from_millis(10),
            context_switcher
        )
    );

    let interrupt_router = static_get_or_init!(InterruptRouter, InterruptRouter::new());

    let mut system_call_table: Vec<&dyn SystemCall> = Vec::with_capacity(64);
    system_call_table.push(static_get_or_init!(SysWrite, SysWrite::new()));

    let system_call_router =
        static_get_or_init!(SystemCallRouter, SystemCallRouter::new(system_call_table));

    let vector_table = static_get_or_init!(
        VectorTable,
        VectorTable::new(interrupt_router, system_call_router)
    );
    vector_table.install();

    let interrupt_controller = Arc::new(Bcm2837InterruptController::new());
    interrupt_router.enable(
        interrupt_controller,
        CNTPNSIRQ,
        Scheduler::schedule,
        scheduler,
    );

    // Initialize the scheduler with a "kinit" task and context switch into it.
    let task_creation = static_get_or_init!(
        TaskCreationService,
        TaskCreationService::new(scheduler, page_allocator)
    );
    task_creation.create_and_become_kinit();
    scheduler.schedule();

    let task_execution =
        static_get_or_init!(TaskExecutionService, TaskExecutionService::new(scheduler));

    let example_thread = |num: u64| {
        let timer = timer.clone();

        let sleep = move |duration: Duration| {
            let target = timer.monotonic().add(duration);

            while timer.monotonic() < target {
                scheduler.schedule();
            }
        };

        move || loop {
            crate::print!("{}", num);
            sleep(Duration::from_secs(num));
        }
    };

    // Create example threads:
    task_creation.create_kthread(example_thread(1));
    task_creation.create_kthread(example_thread(2));

    task_creation.create_kthread(|| {
        task_execution.execute("/bin/example".to_owned());

        Err(())
    });

    kernel_main();

    unimplemented!("shutdown");
}
