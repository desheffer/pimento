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
    pub(super) id: TaskId,
    pub(super) parent_id: ParentTaskId,
    pub(super) name: String,
    pub(super) cpu_context: CpuContext,
    pub(super) memory_context: MemoryContext,
}

impl Task {
    pub(super) fn new(
        parent_id: ParentTaskId,
        name: String,
        cpu_context: CpuContext,
        memory_context: MemoryContext,
    ) -> Self {
        Task {
            id: TaskId::next(),
            parent_id,
            name,
            cpu_context,
            memory_context,
        }
    }
}
