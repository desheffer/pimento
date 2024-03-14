use core::arch::asm;
use core::time::Duration;

use crate::device::{Monotonic, Timer};
use crate::sync::Lock;

const NANOS: u64 = 1_000_000_000;

/// AArch64 timer.
pub struct ArmV8Timer {
    lock: Lock,
}

impl ArmV8Timer {
    /// Creates a timer.
    pub unsafe fn new() -> Self {
        Self { lock: Lock::new() }
    }

    fn frequency(&self) -> u64 {
        // SAFETY: Safe because this is a read operation.
        unsafe {
            let cntfrq: u64;
            asm!("mrs {}, cntfrq_el0", out(reg) cntfrq);
            cntfrq
        }
    }

    fn count(&self) -> u64 {
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
            let tval = (duration.as_nanos() * self.frequency() as u128 / NANOS as u128) as u64;
            asm!("msr cntp_tval_el0, {}", in(reg) tval);

            // Enable the timer.
            asm!("msr cntp_ctl_el0, {}", in(reg) 1u64);
        });
    }
}

impl Monotonic for ArmV8Timer {
    fn monotonic(&self) -> Duration {
        let nanos = (self.count() as u128 * NANOS as u128 / self.frequency() as u128) as u64;
        Duration::from_nanos(nanos)
    }
}
