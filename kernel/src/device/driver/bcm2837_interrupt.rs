use crate::device::InterruptController;
use crate::sync::Lock;

const CORE0_TIMER_IRQCNTL: *mut u32 = 0xFFFF_0000_4000_0040 as *mut _; // Core 0 timers interrupt control
const CORE0_IRQ: *mut u32 = 0xFFFF_0000_4000_0060 as *mut _; // Core 0 IRQ source

pub const CNTPNSIRQ: u64 = 1;

/// Broadcom chip used in the Raspberry Pi 3 Model B and others.
///
/// This implements basic interrupt detection for this chip.
pub struct Bcm2837InterruptController {
    lock: Lock,
}

impl Bcm2837InterruptController {
    /// Creates an interrupt controller driver.
    pub unsafe fn new() -> Self {
        Self { lock: Lock::new() }
    }
}

impl InterruptController for Bcm2837InterruptController {
    fn enable(&self, number: u64) {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            CORE0_TIMER_IRQCNTL
                .write_volatile(CORE0_TIMER_IRQCNTL.read_volatile() | (0b1 << number as u32));
        })
    }

    fn is_pending(&self, number: u64) -> bool {
        // SAFETY: Safe because call is behind a lock.
        self.lock
            .call(|| unsafe { (CORE0_IRQ.read_volatile() & (0b1 << number as u32)) != 0 })
    }

    fn clear(&self, number: u64) {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            CORE0_IRQ.write_volatile(CORE0_IRQ.read_volatile() & !(0b1 << number as u32));
        });
    }
}
