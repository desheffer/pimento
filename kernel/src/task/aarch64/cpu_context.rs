use core::arch::global_asm;

/// AArch64 registers to persist between context switches.
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
    lr: u64, // x29
    pc: u64, // x30
    sp: u64, // x31
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

    pub unsafe fn new_with_task_entry(entry: fn(), sp: *mut u64) -> Self {
        let mut new = Self::new();
        new.set_task_entry(entry);
        new.set_stack_pointer(sp);
        new
    }

    unsafe fn set_task_entry(&mut self, entry: fn()) {
        self.pc = task_entry as *const fn() as u64;
        self.x19 = entry as u64;
    }

    unsafe fn set_stack_pointer(&mut self, sp: *mut u64) {
        self.sp = sp as u64;
    }
}

extern "C" {
    pub fn cpu_context_switch(
        prev: &CpuContext,
        next: &CpuContext,
        after: unsafe extern "C" fn(*const ()),
        data: *const (),
    );

    pub fn task_entry(pc: u64);
}

#[no_mangle]
pub extern "C" fn task_exit() -> ! {
    // TODO: Queue the task for removal.
    loop {}
}

global_asm!(include_str!("cpu_context.s"));
