use core::arch::asm;

const AFFINITY0_MASK: u64 = 0xFF;

/// Gets the index number of the current CPU core.
pub fn current_core() -> usize {
    let mpidr: u64;
    unsafe {
        asm!("mrs {}, mpidr_el1", out(reg) mpidr);
    }
    (mpidr & AFFINITY0_MASK) as usize
}
