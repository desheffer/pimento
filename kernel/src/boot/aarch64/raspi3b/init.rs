use core::ops::Add;
use core::time::Duration;

use alloc::vec;

use crate::abi::{LocalInterruptHandler, VectorTable};
use crate::context::{Scheduler, TaskCreationService};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{Bcm2837InterruptController, CNTPNSIRQ};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::device::{LoggerImpl, Monotonic, MonotonicImpl, TimerImpl};
use crate::kernel_main;
use crate::memory::PageAllocator;
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

    let end = &mut __end as *mut u8 as usize;
    PageAllocator::set_capacity(0x4000_0000);
    PageAllocator::set_reserved_ranges(vec![0..end, 0x3F00_0000..0x4000_0000]);

    Scheduler::set_num_cores(1);
    Scheduler::set_quantum(Duration::from_millis(10));
    TaskCreationService::instance().create_and_become_kinit();

    VectorTable::instance().install();

    let interrupt_controller = Arc::new(Bcm2837InterruptController::new());
    LocalInterruptHandler::instance().enable(interrupt_controller, CNTPNSIRQ, || {
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

/// A simple sleep that burns CPU cycles.
fn sleep(duration: Duration) {
    let monotonic = MonotonicImpl::instance();
    let target = monotonic.monotonic().add(duration);

    while monotonic.monotonic() < target {
        unsafe {
            core::arch::asm!("wfi");
        }
    }
}
