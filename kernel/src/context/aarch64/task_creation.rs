use core::arch::global_asm;

use alloc::borrow::ToOwned;

use crate::context::{ParentTaskId, Scheduler, Task, TaskId};
use crate::cpu::CpuContext;
use crate::memory::MemoryContext;
use crate::sync::OnceLock;

pub struct TaskCreationService<'a> {
    scheduler: &'a Scheduler<'a>,
}

static INSTANCE: OnceLock<TaskCreationService> = OnceLock::new();

impl<'a> TaskCreationService<'a> {
    /// Gets or initializes the task creation service.
    pub fn instance() -> &'static Self {
        INSTANCE.get_or_init(|| Self::new(Scheduler::instance()))
    }

    fn new(scheduler: &'a Scheduler) -> Self {
        Self { scheduler }
    }

    /// Creates and assumes the role of the kernel initialization task. This is necessary because
    /// this task effectively creates itself.
    pub fn create_and_become_kinit(&self) -> TaskId {
        let memory_context;
        unsafe {
            memory_context = MemoryContext::new();
        }

        let cpu_context = CpuContext::zeroed();

        let task = Task::new(
            ParentTaskId::Root,
            "kinit".to_owned(),
            cpu_context,
            memory_context,
        );

        self.scheduler.become_kinit(task)
    }

    /// Spawns a new kernel thread to execute the given function.
    pub fn create_kthread(&self, func: fn()) -> TaskId {
        let mut memory_context;
        unsafe {
            memory_context = MemoryContext::new();
        }

        let mut cpu_context = CpuContext::zeroed();
        unsafe {
            // Set the stack pointer (to the end of the page).
            let kernel_stack = memory_context.alloc_unmapped_page();
            cpu_context.set_sp(kernel_stack.as_mut_ptr().add(1) as usize);

            // Set the program counter by proxy. The `task_raw_entry` function calls `task_start`,
            // which calls `func`.
            cpu_context.set_pc(task_raw_entry as usize, task_start as usize, func as usize);
        }

        let task = Task::new(
            ParentTaskId::Root,
            "kthread".to_owned(),
            cpu_context,
            memory_context,
        );

        self.scheduler.add_task(task)
    }
}

fn task_start(func: fn()) {
    func();

    // TODO: Queue the task for removal.
    unimplemented!("task exit");
}

extern "C" {
    fn task_raw_entry(func: *const ());
}

global_asm!(include_str!("task_creation.s"));
