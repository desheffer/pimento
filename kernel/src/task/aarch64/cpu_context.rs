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
    lr: usize, // x29
    pc: usize, // x30
    sp: usize, // x31
}

impl CpuContext {
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
            lr: 0,
            sp: 0,
            pc: 0,
        }
    }

    pub unsafe fn new_with_task_entry(entry: fn(), sp: *mut u8) -> Self {
        let mut new = Self::new();
        new.set_task_entry(entry);
        new.set_stack_pointer(sp);
        new
    }

    unsafe fn set_task_entry(&mut self, entry: fn()) {
        self.pc = task_entry as *const fn() as usize;
        self.x19 = entry as usize;
    }

    unsafe fn set_stack_pointer(&mut self, sp: *mut u8) {
        self.sp = sp as usize;
    }
}

extern "C" {
    pub fn cpu_context_switch(prev: &CpuContext, next: &CpuContext, after: unsafe extern "C" fn());

    pub fn task_entry(pc: usize);
}

#[no_mangle]
pub extern "C" fn task_exit() -> ! {
    // TODO: Queue the task for removal.
    #[allow(clippy::empty_loop)]
    loop {}
}

global_asm!(include_str!("cpu_context.s"));
