use core::arch::global_asm;

use alloc::borrow::ToOwned;

use crate::context::{ParentTaskId, Scheduler, Task, TaskId};
use crate::cpu::CpuContext;
use crate::memory::{MemoryContext, PageAllocator};

pub struct TaskCreationService {
    scheduler: &'static Scheduler,
    page_allocator: &'static PageAllocator,
}

impl TaskCreationService {
    /// Creates a task creation service.
    pub fn new(scheduler: &'static Scheduler, page_allocator: &'static PageAllocator) -> Self {
        Self {
            scheduler,
            page_allocator,
        }
    }

    /// Creates and assumes the role of the kernel initialization task. This is necessary because
    /// this task effectively creates itself.
    pub unsafe fn create_and_become_kinit(&self) -> TaskId {
        let memory_context = MemoryContext::new(self.page_allocator);

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
    pub unsafe fn create_kthread<T>(&self, func: fn(&T) -> !, data: &'static T) -> TaskId {
        let mut memory_context = MemoryContext::new(self.page_allocator);

        let mut cpu_context = CpuContext::zeroed();

        // Set the stack pointer (to the end of the page).
        let kernel_stack = memory_context.alloc_unmapped_page();
        cpu_context.set_stack_pointer(kernel_stack.as_mut_ptr().add(1) as usize);

        // Set the program counter (link register) by proxy.
        let data_ptr = data as *const _ as *const _;
        cpu_context.set_link_register(cpu_context_entry as usize, func as usize, data_ptr as usize);

        let task = Task::new(
            ParentTaskId::Root,
            "kthread".to_owned(),
            cpu_context,
            memory_context,
        );

        self.scheduler.add_task(task)
    }
}

extern "C" {
    fn cpu_context_entry(func: unsafe extern "C" fn(*const (), ...));
}

global_asm!(include_str!("task_creation.s"));
