use core::arch::global_asm;

#[derive(Debug)]
#[repr(C)]
pub struct CpuContext {
    x: [u64; 11], // x19 - x29
    sp: u64,
    pc: u64,
}

impl CpuContext {
    pub fn new() -> Self {
        Self {
            x: [0; 11],
            sp: 0,
            pc: task_entry as *const fn() as u64,
        }
    }

    pub unsafe fn set_program_counter(&mut self, pc: *const u64) {
        self.x[0] = pc as u64;
    }

    pub unsafe fn set_stack_pointer(&mut self, sp: *mut u64) {
        self.sp = sp as u64;
    }
}

extern "C" {
    pub fn cpu_context_switch(prev: &CpuContext, next: &CpuContext, after: unsafe extern "C" fn());
    pub fn task_entry(pc: u64);
}

#[no_mangle]
pub extern "C" fn task_exit() -> ! {
    // TODO: Queue the task for removal.
    loop {}
}

global_asm!(include_str!("cpu_context.s"));
