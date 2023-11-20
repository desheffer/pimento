use core::cell::UnsafeCell;
use core::time::Duration;

use alloc::borrow::ToOwned;
use alloc::boxed::Box;
use alloc::collections::{BTreeMap, VecDeque};
use alloc::string::String;
use alloc::vec::Vec;

use crate::device::Timer;
use crate::memory::{Page, PageAllocator};
use crate::sync::{Arc, Mutex, UninterruptibleLock};
use crate::task::{cpu_context_switch, CpuContext};

use super::InterruptMask;

/// An auto-incrementing task ID.
#[derive(Clone, Copy, Debug, Eq, Ord, PartialEq, PartialOrd)]
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
#[derive(Debug)]
pub enum ParentTaskId {
    Root,
    TaskId(TaskId),
}

/// A task and the context it needs to run.
#[derive(Debug)]
pub struct Task {
    id: TaskId,
    parent_id: ParentTaskId,
    name: String,
    cpu_context: CpuContext,
    pages: Vec<Page>,
}

impl Task {
    fn new(id: TaskId, parent_id: ParentTaskId, name: String) -> Self {
        Task {
            id,
            parent_id,
            name,
            cpu_context: CpuContext::new(),
            pages: Vec::new(),
        }
    }
}

/// A round-robin task scheduler.
#[derive(Debug)]
pub struct Scheduler {
    lock: UninterruptibleLock,
    tasks: UnsafeCell<BTreeMap<TaskId, Box<Task>>>,
    queue: UnsafeCell<VecDeque<TaskId>>,
    current_task: UnsafeCell<Vec<Option<TaskId>>>,
    quantum: Duration,
    timer: Arc<dyn Timer>,
    page_allocator: Arc<PageAllocator>,
}

impl Scheduler {
    pub fn new(
        num_cores: usize,
        quantum: Duration,
        timer: Arc<dyn Timer>,
        page_allocator: Arc<PageAllocator>,
    ) -> Self {
        let mut current_task = Vec::new();
        current_task.extend((0..num_cores).map(|_| None));

        Self {
            lock: UninterruptibleLock::new(),
            tasks: UnsafeCell::new(BTreeMap::new()),
            queue: UnsafeCell::new(VecDeque::new()),
            current_task: UnsafeCell::new(current_task),
            quantum,
            timer,
            page_allocator,
        }
    }

    /// Creates a task for the "init" process, which is presumed to be the currently running
    /// process. This is required after initialization.
    pub fn create_and_become_init(&self) -> TaskId {
        let tasks = self.tasks.get();
        let queue = self.queue.get();
        let current_task = self.current_task.get();
        let core_num = self.current_core();

        let id = TaskId::next();
        let task = Task::new(id, ParentTaskId::Root, "init".to_owned());

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            assert!((*tasks).is_empty());
            assert!((*queue).is_empty());
            assert!(core_num == 0);

            (*tasks).insert(id, Box::new(task));

            // Skip the queue and set as current task.
            (*current_task)[core_num] = Some(id);
        });

        id
    }

    /// Spawns a new kernel thread that will execute the given function.
    pub fn create_kthread(&self, func: fn()) -> TaskId {
        let tasks = self.tasks.get();
        let queue = self.queue.get();

        let id = TaskId::next();
        let mut task = Task::new(id, ParentTaskId::Root, "kthread".to_owned());

        // SAFETY: Safe because these are basic conversions from references to pointers.
        unsafe {
            task.cpu_context.set_task_entry(func as *const u64);

            let page = self.page_allocator.alloc();
            task.cpu_context
                .set_stack_pointer(page.end_exclusive() as *mut u64);
            task.pages.push(page);
        }

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            (*tasks).insert(id, Box::new(task));
            (*queue).push_back(id);
        });

        id
    }

    /// Determines the next task to run and performs a context switch.
    pub unsafe fn schedule(&self) {
        let tasks = self.tasks.get();
        let queue = self.queue.get();
        let current_task = self.current_task.get();
        let core_num = self.current_core();

        self.lock.lock();

        // Put the current task back into the queue and get the next task.
        let prev_task_id = (*current_task)[core_num].unwrap();
        (*queue).push_back(prev_task_id);
        let next_task_id = (*queue).pop_front().unwrap();
        (*current_task)[core_num] = Some(next_task_id);

        let prev_task = &(*tasks).get(&prev_task_id).unwrap();
        let next_task = &(*tasks).get(&next_task_id).unwrap();

        // SAFETY: The `cpu_context` objects are passed as immutable references. This is safe as
        // long as all access is gated by the exclusive lock.
        cpu_context_switch(
            &prev_task.cpu_context,
            &next_task.cpu_context,
            _scheduler_after_schedule,
            self as *const Scheduler as *const (),
        );

        // The previous function call might not return or might return in a different context. All
        // clean up should be handled before that call or in the `after_schedule` function.
    }

    /// Cleans up after the `schedule` function. This is required due to the complex nature of a
    /// context switch.
    unsafe fn after_schedule(&self) {
        self.lock.unlock();

        self.timer.set_duration(self.quantum);

        InterruptMask::instance().enable_interrupts();
    }

    /// Gets the index number of the current CPU core.
    fn current_core(&self) -> usize {
        // TODO: Only one core is currently supported.
        0
    }
}

/// Wraps the `schedule` function so that it can be called from the interrupt handler.
#[no_mangle]
pub unsafe fn _scheduler_schedule(scheduler: *const ()) {
    let scheduler = &*(scheduler as *const Arc<Scheduler>);
    scheduler.schedule();
}

/// Wraps the `after_schedule` function so that it can be called from the context switch function.
#[no_mangle]
unsafe extern "C" fn _scheduler_after_schedule(scheduler: *const ()) {
    let scheduler = &*(scheduler as *const Scheduler);
    scheduler.after_schedule();
}

unsafe impl Send for Scheduler {}
unsafe impl Sync for Scheduler {}
