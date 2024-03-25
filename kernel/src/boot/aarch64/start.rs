use core::arch::global_asm;

use crate::memory::VA_START;

const HCR_EL2_RW: u64 = 0b1 << 31; // Enable AArch64

const SCTLR_EL1_SA: u64 = 0b1 << 3; // Enable SP alignment check for EL1
const SCTLR_EL1_SA0: u64 = 0b1 << 4; // Enable SP alignment check for EL0
const SCTLR_EL1_EOS: u64 = 0b1 << 11; // Exception exit is context synchronizing
const SCTLR_EL1_NTWI: u64 = 0b1 << 16; // Don't trap EL0 execution of WFI instructions
const SCTLR_EL1_NTWE: u64 = 0b1 << 18; // Don't trap EL0 execution of WFE instructions
const SCTLR_EL1_EIS: u64 = 0b1 << 22; // Exception entry is context synchronizing
const SCTLR_EL1_SPAN: u64 = 0b1 << 23; // Don't enable Privileged Access Never

const SCTLR_EL1_INIT: u64 = SCTLR_EL1_SA
    | SCTLR_EL1_SA0
    | SCTLR_EL1_EOS
    | SCTLR_EL1_NTWI
    | SCTLR_EL1_NTWE
    | SCTLR_EL1_EIS
    | SCTLR_EL1_SPAN;

const SPSR_EL2_F: u64 = 0b1 << 6; // FIQ interrupt mask
const SPSR_EL2_I: u64 = 0b1 << 7; // IRQ interrupt mask
const SPSR_EL2_A: u64 = 0b1 << 8; // Abort (SError) exception mask
const SPSR_EL2_D: u64 = 0b1 << 9; // Debug (Sync) exception mask

const SPSR_EL2_M_EL1H: u64 = 0b0101; // EL1 with SP_EL1 (EL1h)

const SPSR_EL2_INIT: u64 = SPSR_EL2_D | SPSR_EL2_A | SPSR_EL2_I | SPSR_EL2_F | SPSR_EL2_M_EL1H;

global_asm!(
    include_str!("start.s"),
    HCR_EL2_RW = const HCR_EL2_RW,
    SCTLR_EL1_INIT = const SCTLR_EL1_INIT,
    SPSR_EL2_INIT = const SPSR_EL2_INIT,
    VA_START = const VA_START,
);
