use core::arch::global_asm;

/// AArch64 registers to persist between context switches.
#[repr(C)]
pub struct CpuContext {
    x19: usize,
    x20: usize,
    x21: usize,
    x22: usize,
    x23: usize,
    x24: usize,
    x25: usize,
    x26: usize,
    x27: usize,
    x28: usize,
    fp: usize, // x29
    lr: usize, // x30
    sp: usize,
}

impl CpuContext {
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

    pub unsafe fn set_sp(&mut self, sp: usize) {
        self.sp = sp;
    }

    pub unsafe fn set_pc(&mut self, lr: usize, x19: usize, x20: usize) {
        self.lr = lr;
        self.x19 = x19;
        self.x20 = x20;
    }
}

extern "C" {
    pub fn cpu_context_switch(
        prev: &mut CpuContext,
        next: &mut CpuContext,
        after: unsafe extern "C" fn(),
    );
}

global_asm!(include_str!("cpu_context.s"));
