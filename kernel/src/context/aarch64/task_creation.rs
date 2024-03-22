use core::arch::global_asm;

use alloc::borrow::ToOwned;
use alloc::boxed::Box;

use crate::context::{ParentTaskId, Scheduler, Task, TaskId};
use crate::cpu::CpuContext;
use crate::memory::{MemoryContext, PageAllocator};

/// A service for creating new tasks.
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
            Box::new(|| Ok(())),
            cpu_context,
            memory_context,
        );

        self.scheduler.become_kinit(task)
    }

    /// Spawns a new kernel thread to execute the given function.
    pub fn create_kthread<F>(&self, func: F) -> TaskId
    where
        F: Fn() -> Result<(), ()>,
        F: Send + 'static,
    {
        let mut memory_context = MemoryContext::new(self.page_allocator);

        let mut cpu_context = CpuContext::zeroed();

        // Set the stack pointer (to the end of the page).
        let kernel_stack = memory_context.alloc_unmapped_page();
        unsafe {
            cpu_context.set_stack_pointer(kernel_stack.as_mut_ptr().add(1) as _);
        }

        // Wrap the `Fn()` trait so that it can be accessed like a `fn()` pointer.
        unsafe fn fn_trait_wrapper<F>(func: *const F, scheduler: *const Scheduler) -> !
        where
            F: Fn() -> Result<(), ()>,
            F: Send + 'static,
        {
            let _ = (*func)();

            // TODO: Exit the task and remove it from the scheduling queue.
            loop {
                (*scheduler).schedule();
            }
        }

        // Set the program counter (link register) by proxy.
        let func_box = Box::new(func);
        unsafe {
            cpu_context.set_link_register(
                cpu_context_entry as usize as _,
                fn_trait_wrapper::<F> as usize as _,
                func_box.as_ref() as *const _ as _,
                self.scheduler as *const _ as _,
            );
        }

        let task = Task::new(
            ParentTaskId::Root,
            "kthread".to_owned(),
            func_box,
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
