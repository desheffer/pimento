use core::arch::global_asm;

use alloc::borrow::ToOwned;

use crate::context::{ParentTaskId, Scheduler, Task, TaskId};
use crate::cpu::CpuContext;
use crate::memory::PageAllocator;
use crate::sync::OnceLock;

pub struct TaskCreationService<'a> {
    scheduler: &'a Scheduler<'a>,
    page_allocator: &'a PageAllocator,
}

static INSTANCE: OnceLock<TaskCreationService> = OnceLock::new();

impl<'a> TaskCreationService<'a> {
    /// Gets or initializes the task creation service.
    pub fn instance() -> &'static Self {
        INSTANCE.get_or_init(|| Self::new(Scheduler::instance(), PageAllocator::instance()))
    }

    fn new(scheduler: &'a Scheduler, page_allocator: &'a PageAllocator) -> Self {
        Self {
            scheduler,
            page_allocator,
        }
    }

    /// Creates and assumes the role of the kernel initialization task. This is necessary because
    /// this task effectively creates itself.
    pub fn create_and_become_kinit(&self) -> TaskId {
        let id = TaskId::next();
        let cpu_context = CpuContext::zeroed();
        let task = Task::new(
            id,
            ParentTaskId::Root,
            "kinit".to_owned(),
            None,
            cpu_context,
        );

        self.scheduler.become_kinit(task);

        id
    }

    /// Spawns a new kernel thread to execute the given function.
    pub fn create_kthread(&self, func: fn()) -> TaskId {
        // SAFETY: Safe because the page is reserved.
        let mut cpu_context = CpuContext::zeroed();
        let page;
        unsafe {
            // Set stack pointer.
            page = self.page_allocator.alloc();
            let address: usize = page.address().into();
            cpu_context.set_sp(address + page.size());

            // Set program counter by proxy.
            cpu_context.set_pc(task_raw_entry as usize, task_start as usize, func as usize);
        }

        let id = TaskId::next();
        let task = Task::new(
            id,
            ParentTaskId::Root,
            "kthread".to_owned(),
            Some(page),
            cpu_context,
        );

        self.scheduler.add_task(task);

        id
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
