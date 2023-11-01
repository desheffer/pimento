use core::arch::global_asm;

#[derive(Debug)]
#[repr(C)]
pub enum ExecutionLevel {
    EL1t,
    EL1h,
    EL0,
    EL0_32,
}

#[derive(Debug)]
#[repr(C)]
pub enum ExceptionType {
    Sync,
    IRQ,
    FIQ,
    SError,
}

#[no_mangle]
pub extern "C" fn vector_invalid(
    execution_level: ExecutionLevel,
    exception_type: ExceptionType,
    esr_el1: u64,
    far_el1: u64,
) -> ! {
    panic!(
        "invalid vector entry: {:?} {:?} (ESR = {:#018x}, FAR = {:#018x})",
        execution_level, exception_type, esr_el1, far_el1
    );
}

global_asm!(
    include_str!("entry.s"),
    TYPE_SYNC = const ExceptionType::Sync as u64,
    TYPE_IRQ = const ExceptionType::IRQ as u64,
    TYPE_FIQ = const ExceptionType::FIQ as u64,
    TYPE_SERROR = const ExceptionType::SError as u64,
    LEVEL_EL1T = const ExecutionLevel::EL1t as u64,
    LEVEL_EL1H = const ExecutionLevel::EL1h as u64,
    LEVEL_EL0 = const ExecutionLevel::EL0 as u64,
    LEVEL_EL0_32 = const ExecutionLevel::EL0_32 as u64,
);
