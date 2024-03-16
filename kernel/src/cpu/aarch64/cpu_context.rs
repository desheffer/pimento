use core::arch::global_asm;

/// The AArch64 registers to persist between context switches.
#[repr(C)]
pub struct CpuContext {
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

impl CpuContext {
    /// Creates a CPU context initialized with all zeroes.
    pub fn zeroed() -> Self {
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

    /// Sets the stack pointer.
    pub unsafe fn set_stack_pointer(&mut self, sp: usize) {
        assert!(self.sp == 0);
        self.sp = sp as u64;
    }

    /// Sets the link register.
    pub unsafe fn set_link_register(&mut self, lr: usize, x19: usize, x20: usize) {
        assert!(self.lr == 0);
        self.lr = lr as u64;
        self.x19 = x19 as u64;
        self.x20 = x20 as u64;
    }
}

extern "C" {
    /// Performs the CPU-specific steps of a context switch.
    pub fn cpu_context_switch(
        prev: &mut CpuContext,
        next: &mut CpuContext,
        after_func: unsafe extern "C" fn(*const ()),
        after_data: *const (),
    );
}

global_asm!(include_str!("cpu_context.s"));
