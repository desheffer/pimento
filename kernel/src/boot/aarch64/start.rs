use core::arch::global_asm;

const HCR_EL2_RW: u64 = 0b1 << 31; // Enable AArch64

const SPSR_EL2_M_EL1H: u64 = 0b0101 << 0; // EL1 with SP_EL1 (EL1h)

const SPSR_EL2_D: u64 = 0b1 << 9; // Debug (Sync) exception mask
const SPSR_EL2_A: u64 = 0b1 << 8; // Abort (SError) exception mask
const SPSR_EL2_I: u64 = 0b1 << 7; // IRQ interrupt mask
const SPSR_EL2_F: u64 = 0b1 << 6; // FIQ interrupt mask

const SPSR_EL2_INIT: u64 = SPSR_EL2_D | SPSR_EL2_A | SPSR_EL2_I | SPSR_EL2_F | SPSR_EL2_M_EL1H;

global_asm!(
    include_str!("start.s"),
    HCR_EL2_RW = const HCR_EL2_RW,
    SPSR_EL2_INIT = const SPSR_EL2_INIT,
);
