use core::arch::asm;
use core::time::Duration;

use crate::device::{Counter, Timer};
use crate::sync::Lock;

const NANOS: u64 = 1_000_000_000;

/// AArch64 timer
#[derive(Debug)]
pub struct ArmV8Timer {
    lock: Lock,
}

impl ArmV8Timer {
    pub const unsafe fn new() -> Self {
        Self { lock: Lock::new() }
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

impl Timer for ArmV8Timer {
    fn set_duration(&self, duration: Duration) {
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
}

impl Counter for ArmV8Timer {
    fn uptime(&self) -> Duration {
        let nanos = (self.counter() as u128 * NANOS as u128 / self.counter_freq() as u128) as u64;
        Duration::from_nanos(nanos)
    }
}
