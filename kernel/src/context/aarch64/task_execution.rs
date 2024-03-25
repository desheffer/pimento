use core::arch::global_asm;

use alloc::string::String;

use crate::context::Scheduler;

const SPSR_EL1_M_EL0T: u64 = 0b0000; // EL0 with SP_EL0 (EL0t)

const SPSR_EL1_INIT_EL0: u64 = SPSR_EL1_M_EL0T;

/// A service for executing code.
pub struct TaskExecutionService {
    scheduler: &'static Scheduler,
}

impl TaskExecutionService {
    /// Creates a task execution service.
    pub fn new(scheduler: &'static Scheduler) -> Self {
        Self { scheduler }
    }

    /// Executes a user program in the current context.
    pub fn execute(&self, _path: String) {
        // We will ignore `path` for now and instead use the linked example code.
        extern "C" {
            static user_code_start: u8;
            static user_code_end: u8;
        }

        // Jump into the user context.
        // SAFETY: Safe because the entry point is allocated.
        unsafe {
            enter_el0(&user_code_start as *const _ as _);
        }
    }
}

extern "C" {
    fn enter_el0(entry: *const ());
}

global_asm!(
    include_str!("task_execution.s"),
    SPSR_EL1_INIT_EL0 = const SPSR_EL1_INIT_EL0,
);
