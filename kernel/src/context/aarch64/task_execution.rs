use core::arch::{asm, global_asm};
use core::mem::size_of;

use crate::context::Scheduler;
use crate::fs::{FileManager, PathInfo};
use crate::memory::{Page, UserVirtualAddress};

const SPSR_EL1_M_EL0T: u64 = 0b0000; // EL0 with SP_EL0 (EL0t)

const SPSR_EL1_INIT_EL0: u64 = SPSR_EL1_M_EL0T;

/// A service for executing code.
pub struct TaskExecutionService {
    scheduler: &'static Scheduler,
    file_manager: &'static FileManager,
}

impl TaskExecutionService {
    /// Creates a task execution service.
    pub fn new(scheduler: &'static Scheduler, file_manager: &'static FileManager) -> Self {
        Self {
            scheduler,
            file_manager,
        }
    }

    /// Executes a user program in the current context.
    pub fn execute(&self, path: &PathInfo) -> Result<(), ()> {
        // Get the current task.
        let task_id = self.scheduler.current_task_id();
        let task = self.scheduler.task(task_id).unwrap();

        // The program entry point and the address of each section will be set by the ELF header
        // (when we can load ELF files). For now, almost any value should work.
        let user_entry = UserVirtualAddress::<Page>::new(0x1000_0000);

        // Open the user binary and copy it into the user context.
        let file = self.file_manager.open(path)?;
        unsafe {
            let mut copy_dest = user_entry;

            loop {
                let binary = file.read(size_of::<Page>())?;
                if binary.is_empty() {
                    break;
                }

                task.memory_context.alloc_page(copy_dest)?;

                task.memory_context.copy_to_user(
                    binary.as_ptr(),
                    copy_dest.convert(),
                    binary.len(),
                )?;

                copy_dest = copy_dest.add(1);
            }
        }

        // The stack starting address will be set at a later point. For now, almost any value
        // higher than `user_entry` should work.
        let stack_start = UserVirtualAddress::<Page>::new(0x8000_0000);

        // Create a stack for the user context (where `stack_start` is the end of the page).
        unsafe {
            let stack_end = UserVirtualAddress::<Page>::new(stack_start.sub(1).as_ptr() as _);
            task.memory_context.alloc_page(stack_end)?;

            let sp_el0 = stack_start.as_ptr();
            asm!("msr sp_el0, {}", in(reg) sp_el0)
        }

        // Jump into the user context.
        unsafe {
            enter_el0(user_entry.as_ptr() as _);
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
