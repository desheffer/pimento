use crate::device::InterruptController;
use crate::sync::Lock;

const CONTROL: *mut u32 = 0x40000000 as *mut u32; // Control register
const CORE_TIMER_PRESCALER: *mut u32 = 0x40000008 as *mut u32; // Core timer prescaler
const CORE_TIMER_LS: *mut u32 = 0x4000001C as *mut u32; // Core timer access LS
const CORE_TIMER_MS: *mut u32 = 0x40000020 as *mut u32; // Core timer access MS
const CORE0_TIMER_IRQCNTL: *mut u32 = 0x40000040 as *mut u32; // Core 0 timers interrupt control
const CORE1_TIMER_IRQCNTL: *mut u32 = 0x40000044 as *mut u32; // Core 1 timers interrupt control
const CORE2_TIMER_IRQCNTL: *mut u32 = 0x40000048 as *mut u32; // Core 2 timers interrupt control
const CORE3_TIMER_IRQCNTL: *mut u32 = 0x4000004C as *mut u32; // Core 3 timers interrupt control
const CORE0_IRQ: *mut u32 = 0x40000060 as *mut u32; // Core 0 IRQ source
const CORE1_IRQ: *mut u32 = 0x40000064 as *mut u32; // Core 1 IRQ source
const CORE2_IRQ: *mut u32 = 0x40000068 as *mut u32; // Core 2 IRQ source
const CORE3_IRQ: *mut u32 = 0x4000006C as *mut u32; // Core 3 IRQ source

pub const CNTPNSIRQ: u64 = 1;

/// Broadcom chip used in the Raspberry Pi 3 Model B and others.
///
/// This implements basic interrupt detection for this chip.
pub struct Bcm2837InterruptController {
    lock: Lock,
}

impl Bcm2837InterruptController {
    pub const unsafe fn new() -> Self {
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
