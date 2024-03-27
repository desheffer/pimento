use core::arch::{asm, global_asm};
use core::ptr::addr_of;

use alloc::string::String;

use crate::context::Scheduler;
use crate::memory::{Page, UserVirtualAddress};

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
    ///
    /// This is a work-in-progress that runs a contrived example program.
    pub fn execute(&self, _path: String) -> Result<(), ()> {
        // Get the current task.
        let task_id = self.scheduler.current_task_id();
        let task = self.scheduler.task(task_id).unwrap();

        // We will ignore `path` (for now) and use the linked example code instead.
        extern "C" {
            static user_code_start: u8;
            static user_code_end: u8;
        }

        // The program entry point and the address of each section will be set by the ELF header
        // (when we can load ELF files). For now, almost any value should work.
        let user_entry = UserVirtualAddress::<u8>::new(0x1000_0000);

        // Copy the user code into the user context.
        unsafe {
            task.memory_context.alloc_page(user_entry)?;

            task.memory_context.copy_to_user(
                addr_of!(user_code_start),
                user_entry,
                addr_of!(user_code_end) as usize - addr_of!(user_code_start) as usize,
            );
        }

        // The stack starting address will be set at a later point. For now, almost any value
        // higher than `user_entry` should work.
        let stack_start = UserVirtualAddress::<Page>::new(0x8000_0000);

        // Create a stack for the user context (where `stack_start` is the end of the page).
        unsafe {
            let stack_end = UserVirtualAddress::<Page>::new(stack_start.ptr().sub(1) as _);
            task.memory_context.alloc_page(stack_end)?;

            let sp_el0 = stack_start.ptr();
            asm!("msr sp_el0, {}", in(reg) sp_el0)
        }

        // Jump into the user context.
        unsafe {
            enter_el0(user_entry.ptr() as _);
        }
    }
}

extern "C" {
    fn enter_el0(entry: *const ()) -> !;
}

global_asm!(
    include_str!("task_execution.s"),
    SPSR_EL1_INIT_EL0 = const SPSR_EL1_INIT_EL0,
);
