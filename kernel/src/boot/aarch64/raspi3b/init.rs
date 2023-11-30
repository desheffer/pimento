use core::ops::Add;
use core::time::Duration;

use alloc::vec;

use crate::abi::{LocalInterruptHandler, VectorTable};
use crate::device::driver::armv8_timer::ArmV8Timer;
use crate::device::driver::bcm2837_interrupt::{Bcm2837InterruptController, CNTPNSIRQ};
use crate::device::driver::bcm2837_serial::Bcm2837Serial;
use crate::device::{LoggerImpl, Monotonic, MonotonicImpl, TimerImpl};
use crate::kernel_main;
use crate::memory::PageAllocator;
use crate::sync::Arc;
use crate::task::{Scheduler, _scheduler_schedule};

const QUANTUM: Duration = Duration::from_millis(10);

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
    PageAllocator::set_page_size(4096);
    PageAllocator::set_capacity(0x40000000);
    PageAllocator::set_reserved_ranges(vec![0..end, 0x3F000000..0x40000000]);

    VectorTable::instance().install();

    Scheduler::set_num_cores(1);
    Scheduler::set_quantum(QUANTUM);
    Scheduler::instance().create_and_become_kinit();

    let interrupt_controller = Arc::new(Bcm2837InterruptController::new());
    LocalInterruptHandler::instance().enable(interrupt_controller, CNTPNSIRQ, _scheduler_schedule);

    Scheduler::instance().schedule();

    // Create example threads:
    Scheduler::instance().create_kthread(|| loop {
        crate::print!("1");
        sleep(Duration::from_secs(1));
    });
    Scheduler::instance().create_kthread(|| loop {
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
