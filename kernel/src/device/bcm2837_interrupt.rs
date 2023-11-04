use crate::interrupt::Interrupt;
use crate::sync::{Lock, OnceLock};

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

/// Broadcom chip used in the Raspberry Pi 3 Model B and others
///
/// This implements basic interrupt detection for this chip.
#[derive(Debug)]
pub struct BCM2837InterruptController {
    lock: Lock,
}

impl BCM2837InterruptController {
    pub fn instance() -> &'static Self {
        static INSTANCE: OnceLock<BCM2837InterruptController> = OnceLock::new();
        INSTANCE.get_or_init(|| Self::new())
    }

    fn new() -> Self {
        Self { lock: Lock::new() }
    }

    fn enable(&self, number: u32) {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            CORE0_TIMER_IRQCNTL
                .write_volatile(CORE0_TIMER_IRQCNTL.read_volatile() | (0b1 << number));
        })
    }

    fn pending(&self, number: u32) -> bool {
        // SAFETY: Safe because call is behind a lock.
        self.lock
            .call(|| unsafe { (CORE0_IRQ.read_volatile() & (0b1 << number)) != 0 })
    }

    fn clear(&self, number: u32) {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            CORE0_IRQ.write_volatile(CORE0_IRQ.read_volatile() & !(0b1 << number));
        });
    }
}

#[derive(Debug)]
pub struct BCM2837Interrupt {
    number: u32,
}

impl BCM2837Interrupt {
    const fn new(number: u32) -> Self {
        Self { number }
    }
}

impl Interrupt for BCM2837Interrupt {
    fn enable(&self) {
        BCM2837InterruptController::instance().enable(self.number);
    }

    fn is_pending(&self) -> bool {
        BCM2837InterruptController::instance().pending(self.number)
    }

    fn clear(&self) {
        BCM2837InterruptController::instance().clear(self.number);
    }
}

pub static CNTPSIRQ: BCM2837Interrupt = BCM2837Interrupt::new(0);
pub static CNTPNSIRQ: BCM2837Interrupt = BCM2837Interrupt::new(1);
pub static CNTHPIRQ: BCM2837Interrupt = BCM2837Interrupt::new(2);
pub static CNTVIRQ: BCM2837Interrupt = BCM2837Interrupt::new(3);
pub static MAILBOX0: BCM2837Interrupt = BCM2837Interrupt::new(4);
pub static MAILBOX1: BCM2837Interrupt = BCM2837Interrupt::new(5);
pub static MAILBOX2: BCM2837Interrupt = BCM2837Interrupt::new(6);
pub static MAILBOX3: BCM2837Interrupt = BCM2837Interrupt::new(7);
pub static GPU_FAST: BCM2837Interrupt = BCM2837Interrupt::new(8);
pub static PMU_FAST: BCM2837Interrupt = BCM2837Interrupt::new(9);
