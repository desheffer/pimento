use core::arch::asm;
use core::time::Duration;

use crate::sync::OnceLock;

const NANOS: u64 = 1_000_000_000;

#[derive(Debug)]
pub struct Counter;

impl Counter {
    pub fn instance() -> &'static Self {
        static INSTANCE: OnceLock<Counter> = OnceLock::new();
        INSTANCE.get_or_init(|| Self::new())
    }

    fn new() -> Self {
        Self
    }

    pub fn uptime(&self) -> Duration {
        let nanos = (self.counter() as u128 * NANOS as u128 / self.counter_freq() as u128) as u64;
        Duration::from_nanos(nanos)
    }

    fn counter_freq(&self) -> u64 {
        // SAFETY: Safe because this is a read operation.
        unsafe {
            let cntfrq: u64;
            asm!("mrs {}, cntfrq_el0", out(reg) cntfrq);
            cntfrq
        }
    }

    fn counter(&self) -> u64 {
        // SAFETY: Safe because this is a read operation.
        unsafe {
            let cntpct: u64;
            asm!("mrs {}, cntpct_el0", out(reg) cntpct);
            cntpct
        }
    }
}
