use core::sync::atomic::AtomicU64;
use core::sync::atomic::Ordering::Relaxed;

use alloc::boxed::Box;
use alloc::fmt;
use alloc::string::String;

use crate::cpu::CpuContext;
use crate::fs::FsContext;
use crate::memory::MemoryContext;

/// An auto-incrementing task ID.
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub struct TaskId {
    id: u64,
}

impl TaskId {
    /// Generates the next available task ID.
    pub fn next() -> Self {
        static COUNTER: AtomicU64 = AtomicU64::new(1);
        Self {
            id: COUNTER.fetch_add(1, Relaxed),
        }
    }
}

impl fmt::Display for TaskId {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}", self.id)
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
    pub fs_context: FsContext,
}

impl Task {
    /// Creates a task.
    pub(super) fn new(
        parent_id: ParentTaskId,
        name: String,
        func: Option<Box<dyn Fn() -> Result<(), ()>>>,
        cpu_context: CpuContext,
        memory_context: MemoryContext,
        fs_context: FsContext,
    ) -> Self {
        Task {
            id: TaskId::next(),
            parent_id,
            name,
            func,
            cpu_context,
            memory_context,
            fs_context,
        }
    }
}
