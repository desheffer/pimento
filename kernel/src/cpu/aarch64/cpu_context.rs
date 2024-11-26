use core::arch::global_asm;
use core::cell::UnsafeCell;

/// The AArch64 registers to save between context switches.
#[repr(C)]
pub struct Registers {
    x19: u64,
    x20: u64,
    x21: u64,
    x22: u64,
    x23: u64,
    x24: u64,
    x25: u64,
    x26: u64,
    x27: u64,
    x28: u64,
    fp: u64, // x29
    lr: u64, // x30
    sp: u64,
}

impl Registers {
    /// Creates a registers record initialized with all zeroes.
    pub fn new() -> Self {
        Self {
            x19: 0,
            x20: 0,
            x21: 0,
            x22: 0,
            x23: 0,
            x24: 0,
            x25: 0,
            x26: 0,
            x27: 0,
            x28: 0,
            fp: 0,
            lr: 0,
            sp: 0,
        }
    }
}

/// The AArch64 CPU context for a task.
pub struct CpuContext {
    pub registers: UnsafeCell<Registers>,
}

impl CpuContext {
    /// Creates a CPU context.
    pub fn new() -> Self {
        Self {
            registers: UnsafeCell::new(Registers::new()),
        }
    }

    /// Sets the stack pointer.
    pub unsafe fn set_stack_pointer(&mut self, sp: u64) {
        (*self.registers.get()).sp = sp as _;
    }

    /// Sets the link register.
    pub unsafe fn set_link_register(&mut self, lr: u64, x19: u64, x20: u64) {
        (*self.registers.get()).lr = lr;
        (*self.registers.get()).x19 = x19;
        (*self.registers.get()).x20 = x20;
    }
}

extern "C" {
    /// Performs the CPU-specific steps of a context switch.
    pub fn cpu_context_switch(
        prev: *mut Registers,
        next: *mut Registers,
        after_func: unsafe extern "C" fn(*const ()),
        after_data: *const (),
    );
}

global_asm!(include_str!("cpu_context.s"));
