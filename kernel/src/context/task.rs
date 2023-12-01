use alloc::string::String;

use crate::context::CpuContext;
use crate::memory::Page;
use crate::sync::Mutex;

/// An auto-incrementing task ID.
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub struct TaskId {
    id: u64,
}

impl TaskId {
    pub fn next() -> Self {
        static NEXT: Mutex<u64> = Mutex::new(1);
        let mut next = NEXT.lock();
        let task_id = TaskId { id: *next };
        *next += 1;
        task_id
    }
}

/// A task's parent task ID.
pub enum ParentTaskId {
    Root,
    TaskId(TaskId),
}

/// A task and the context it needs to run.
pub struct Task {
    pub(super) id: TaskId,
    pub(super) parent_id: ParentTaskId,
    pub(super) name: String,
    pub(super) kernel_stack: Option<Page>,
    pub(super) cpu_context: CpuContext,
}

impl Task {
    pub(super) fn new(
        id: TaskId,
        parent_id: ParentTaskId,
        name: String,
        kernel_stack: Option<Page>,
        cpu_context: CpuContext,
    ) -> Self {
        Task {
            id,
            parent_id,
            name,
            kernel_stack,
            cpu_context,
        }
    }
}
