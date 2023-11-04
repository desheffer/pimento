use core::arch::asm;
use core::time::Duration;

use crate::sync::{Lock, OnceLock};

const NANOS: u64 = 1_000_000_000;

#[derive(Debug)]
pub struct Timer {
    lock: Lock,
}

impl Timer {
    pub fn instance() -> &'static Self {
        static INSTANCE: OnceLock<Timer> = OnceLock::new();
        INSTANCE.get_or_init(|| Self::new())
    }

    fn new() -> Self {
        Self { lock: Lock::new() }
    }

    pub fn set_timer(&self, duration: Duration) {
        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            // Set the timer.
            let tval =
                (duration.as_nanos() as u128 * self.counter_freq() as u128 / NANOS as u128) as u64;
            asm!("msr cntp_tval_el0, {}", in(reg) tval);

            // Enable the timer.
            asm!("msr cntp_ctl_el0, {}", in(reg) 1u64);
        });
    }

    fn counter_freq(&self) -> u64 {
        // SAFETY: Safe because this is a read operation.
        unsafe {
            let cntfrq: u64;
            asm!("mrs {}, cntfrq_el0", out(reg) cntfrq);
            cntfrq
        }
    }
}
