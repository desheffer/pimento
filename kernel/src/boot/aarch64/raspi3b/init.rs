use core::ptr::addr_of;
use core::time::Duration;

use alloc::sync::Arc;
use alloc::vec::Vec;

use crate::abi::{InterruptRouter, SysWrite, SystemCall, SystemCallRouter, VectorTable};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{Bcm2837InterruptController, CNTPNSIRQ};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::device::Devfs;
use crate::fs::{FileManager, PathInfo, Tmpfs, VirtualFileSystem};
use crate::kernel::Kernel;
use crate::log;
use crate::memory::{PageAllocator, PhysicalAddress, MEMORY_MAPPER};
use crate::sync::OnceLock;
use crate::task::{self, Scheduler, TaskCreationService, TaskExecutionService};
use crate::{print, static_get_or_init};

extern "C" {
    static __end: u8;
}

/// Initializes a Raspberry Pi 3 B.
///
/// This initialization sequence is designed so that, at some point in the future, it might be
/// loaded from a Device Tree Blob (DTB).
#[no_mangle]
pub unsafe extern "C" fn kernel_init() -> ! {
    let serial = Arc::new(Bcm2837Serial::new());
    serial.init();
    log::set_character_device(serial.clone()).unwrap();

    let timer = Arc::new(ArmV8Timer::new());
    log::set_monotonic(timer.clone()).unwrap();

    let page_allocator = static_get_or_init!(
        PageAllocator,
        PageAllocator::new(
            0x4000_0000,
            &[
                PhysicalAddress::new(0)..MEMORY_MAPPER.physical_address(addr_of!(__end) as _),
                PhysicalAddress::new(0x3F00_0000)..PhysicalAddress::new(0x4000_0000),
            ],
        )
    );

    let file_system = static_get_or_init!(VirtualFileSystem, VirtualFileSystem::new());
    let file_manager = static_get_or_init!(FileManager, FileManager::new(file_system));

    // Mount the root file system.
    let root = Tmpfs::new();
    file_manager
        .mount(&PathInfo::absolute("/").unwrap(), root)
        .unwrap();

    // Mount the device file system.
    let dev_path = PathInfo::absolute("/dev").unwrap();
    file_manager.mkdir(&dev_path).unwrap();
    let devfs = Devfs::new();
    file_manager.mount(&dev_path, devfs.clone()).unwrap();

    // Register the serial device.
    devfs
        .register_character_device("ttyS0", serial.clone())
        .unwrap();

    // Create the bin directory.
    let bin_path = PathInfo::absolute("/bin").unwrap();
    file_manager.mkdir(&bin_path).unwrap();

    // Register the CLI application.
    let cli_path = PathInfo::absolute("/bin/cli").unwrap();
    file_manager.mknod(&cli_path).unwrap();
    let cli = file_manager.open(&cli_path).unwrap();
    let cli_bytes =
        include_bytes!("../../../../../cli/target/aarch64-unknown-none-softfloat/release/cli");
    cli.write(cli_bytes).unwrap();

    // TODO: Support multiple cores.
    let num_cores = 1;
    let scheduler = static_get_or_init!(
        Scheduler,
        Scheduler::new(num_cores, timer.clone(), Duration::from_millis(10))
    );
    task::set_scheduler(scheduler).unwrap();

    let interrupt_router = static_get_or_init!(InterruptRouter, InterruptRouter::new());

    let mut system_call_table: Vec<&dyn SystemCall> = Vec::with_capacity(64);
    system_call_table.push(static_get_or_init!(SysWrite, SysWrite::new(scheduler)));

    let system_call_router =
        static_get_or_init!(SystemCallRouter, SystemCallRouter::new(&system_call_table));

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

    let task_execution = static_get_or_init!(
        TaskExecutionService,
        TaskExecutionService::new(scheduler, file_manager)
    );

    for _ in 0..2 {
        task_creation
            .create_kthread(|| loop {
                print!("<k{}>", task::current_task().id);
                for _ in 0..100_000_000 {
                    core::arch::asm!("nop");
                }
            })
            .unwrap();
    }

    // Initialization is complete. Run the kernel.
    let kernel = Kernel::new(task_execution);
    kernel.run().unwrap();

    unimplemented!("shutdown");
}
