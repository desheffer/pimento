use core::arch::global_asm;

const HCR_EL2_RW: u64 = 0b1 << 31; // Enable AArch64

const SPSR_EL2_M_EL1H: u64 = 0b0101 << 0; // EL1 with SP_EL1 (EL1h)

global_asm!(
    include_str!("start.s"),
    HCR_EL2_RW = const HCR_EL2_RW,
    SPSR_EL2_M_EL1H = const SPSR_EL2_M_EL1H,
);
