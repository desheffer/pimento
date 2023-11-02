use core::arch::asm;
use core::time::Duration;

const NANOS: u64 = 1_000_000_000;

pub fn uptime() -> Duration {
    let nanos = (counter() as u128 * NANOS as u128 / counter_freq() as u128) as u64;
    Duration::from_nanos(nanos)
}

fn counter_freq() -> u64 {
    let cntfrq: u64;

    // SAFETY: Safe because this is a read operation.
    unsafe {
        asm!("mrs {}, cntfrq_el0", out(reg) cntfrq);
    }

    cntfrq
}

fn counter() -> u64 {
    let cntpct: u64;

    // SAFETY: Safe because this is a read operation.
    unsafe {
        asm!("mrs {}, cntpct_el0", out(reg) cntpct);
    }

    cntpct
}

pub unsafe fn set_timer(duration: Duration) {
    // Set the timer.
    let tval = (duration.as_nanos() as u128 * counter_freq() as u128 / NANOS as u128) as u64;
    asm!("msr cntp_tval_el0, {}", in(reg) tval);

    // Enable the timer.
    asm!("msr cntp_ctl_el0, {}", in(reg) 1u64);
}
