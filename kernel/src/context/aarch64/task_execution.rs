use core::arch::{asm, global_asm};
use core::mem::transmute;

use crate::context::{Elf64Reader, Scheduler};
use crate::fs::{FileManager, PathInfo};
use crate::memory::{Page, UserVirtualAddress};

const SPSR_EL1_M_EL0T: u64 = 0b0000; // EL0 with SP_EL0 (EL0t)

const SPSR_EL1_INIT_EL0: u64 = SPSR_EL1_M_EL0T;

const STACK_INIT: usize = 0x8000_0000;

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

        // Open the binary file.
        let file = self.file_manager.open(path)?;
        let reader = Elf64Reader::new(file)?;

        // Copy each section from the binary file.
        for loadable_page in reader.loadable_pages() {
            task.memory_context.alloc_page(loadable_page.address())?;

            unsafe {
                task.memory_context.copy_to_user(
                    loadable_page.bytes().as_ptr(),
                    loadable_page.address(),
                    loadable_page.bytes().len(),
                )?;
            }
        }

        // Create a stack for the user context (where `STACK_INIT` is the starting address of the
        // following page).
        unsafe {
            let stack_init = UserVirtualAddress::<Page>::new(STACK_INIT);
            task.memory_context.alloc_page(stack_init.sub(1))?;

            asm!("msr sp_el0, {}", in(reg) stack_init.as_ptr());
        }

        // Jump into the user context.
        unsafe {
            let entry_point = transmute::<usize, extern "C" fn()>(reader.entry_point());
            enter_el0(entry_point);
        }
    }
}

extern "C" {
    fn enter_el0(entry: extern "C" fn()) -> !;
}

global_asm!(
    include_str!("task_execution.s"),
    SPSR_EL1_INIT_EL0 = const SPSR_EL1_INIT_EL0,
);
