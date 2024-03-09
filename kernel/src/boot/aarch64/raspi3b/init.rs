use core::ops::Add;
use core::ptr::addr_of;
use core::time::Duration;

use alloc::vec;

use crate::abi::{register_system_calls, InterruptRouter, VectorTable};
use crate::context::{Scheduler, TaskCreationService};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{Bcm2837InterruptController, CNTPNSIRQ};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::device::{LoggerImpl, Monotonic, MonotonicImpl, TimerImpl};
use crate::kernel_main;
use crate::memory::{PageAllocator, PhysicalAddress};
use crate::sync::Arc;

extern "C" {
    static mut __end: u8;
}

/// Initializes a Raspberry Pi 3 B.
///
/// This initialization sequence is designed so that, at some point in the future, it might be
/// loaded from a Device Tree Blob (DTB).
#[no_mangle]
pub unsafe extern "C" fn kernel_init() -> ! {
    let serial = Arc::new(Bcm2837Serial::new());
    serial.init();
    LoggerImpl::instance().set_inner(serial.clone());

    let timer = Arc::new(ArmV8Timer::new());
    MonotonicImpl::instance().set_inner(timer.clone());
    TimerImpl::instance().set_inner(timer.clone());

    PageAllocator::set_capacity(0x4000_0000);
    PageAllocator::set_reserved_ranges(vec![
        PhysicalAddress::new(0)..PhysicalAddress::from_ptr(addr_of!(__end)),
        PhysicalAddress::new(0x3F00_0000)..PhysicalAddress::new(0x4000_0000),
    ]);

    Scheduler::set_num_cores(1);
    Scheduler::set_quantum(Duration::from_millis(10));
    TaskCreationService::instance().create_and_become_kinit();

    register_system_calls();
    VectorTable::instance().install();

    let interrupt_controller = Arc::new(Bcm2837InterruptController::new());
    InterruptRouter::instance().enable(interrupt_controller, CNTPNSIRQ, || {
        Scheduler::instance().schedule()
    });

    Scheduler::instance().schedule();

    // Create example threads:
    TaskCreationService::instance().create_kthread(|| loop {
        crate::print!("1");
        sleep(Duration::from_secs(1));
    });
    TaskCreationService::instance().create_kthread(|| loop {
        crate::print!("2");
        sleep(Duration::from_secs(2));
    });

    kernel_main();

    unimplemented!("shutdown");
}

/// Burns CPU cycles for the given Duration.
fn sleep(duration: Duration) {
    let monotonic = MonotonicImpl::instance();
    let target = monotonic.monotonic().add(duration);

    while monotonic.monotonic() < target {
        unsafe {
            core::arch::asm!("wfi");
        }
    }
}
