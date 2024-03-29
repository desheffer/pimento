use alloc::boxed::Box;
use alloc::string::String;

use crate::cpu::CpuContext;
use crate::memory::MemoryContext;
use crate::sync::Mutex;

/// An auto-incrementing task ID.
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub struct TaskId {
    id: u64,
}

impl TaskId {
    /// Generates the next available task ID.
    pub fn next() -> Self {
        static NEXT: Mutex<u64> = Mutex::new(1);
        let mut next = NEXT.lock();
        let id = *next;
        *next += 1;
        Self { id }
    }
}

/// A task's parent task ID.
pub enum ParentTaskId {
    Root,
    TaskId(TaskId),
}

/// A task and the context it needs to run.
pub struct Task {
    pub id: TaskId,
    pub parent_id: ParentTaskId,
    pub name: String,
    pub func: Option<Box<dyn Fn() -> Result<(), ()>>>,
    pub cpu_context: CpuContext,
    pub memory_context: MemoryContext,
}

impl Task {
    /// Creates a task.
    pub(super) fn new(
        parent_id: ParentTaskId,
        name: String,
        func: Option<Box<dyn Fn() -> Result<(), ()>>>,
        cpu_context: CpuContext,
        memory_context: MemoryContext,
    ) -> Self {
        Task {
            id: TaskId::next(),
            parent_id,
            name,
            func,
            cpu_context,
            memory_context,
        }
    }
}
