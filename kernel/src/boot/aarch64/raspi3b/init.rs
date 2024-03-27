use core::ptr::addr_of;
use core::time::Duration;

use alloc::vec;
use alloc::vec::Vec;

use crate::abi::{InterruptRouter, SysWrite, SystemCall, SystemCallRouter, VectorTable};
use crate::context::{ContextSwitch, Scheduler, TaskCreationService, TaskExecutionService};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{Bcm2837InterruptController, CNTPNSIRQ};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::kernel::Kernel;
use crate::memory::{PageAllocator, PhysicalAddress, MEMORY_MAPPER};
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
    print::set_logger(serial.clone()).unwrap();

    let timer = Arc::new(ArmV8Timer::new());
    print::set_monotonic(timer.clone()).unwrap();

    let page_allocator = static_get_or_init!(
        PageAllocator,
        PageAllocator::new(
            0x4000_0000,
            vec![
                PhysicalAddress::new(0)..MEMORY_MAPPER.physical_address(addr_of!(__end) as _),
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
    system_call_table.push(static_get_or_init!(SysWrite, SysWrite::new(scheduler)));

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
    task_creation.create_and_become_kinit().unwrap();
    scheduler.schedule();

    let task_execution =
        static_get_or_init!(TaskExecutionService, TaskExecutionService::new(scheduler));

    // Initialization is complete. Run the kernel.
    let kernel = Kernel::new(task_execution);
    kernel.run().unwrap();

    unimplemented!("shutdown");
}
