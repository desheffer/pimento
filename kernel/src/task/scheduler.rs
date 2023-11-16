use core::cell::UnsafeCell;

use alloc::borrow::ToOwned;
use alloc::boxed::Box;
use alloc::collections::{BTreeMap, VecDeque};
use alloc::string::String;
use alloc::vec::Vec;

use crate::memory::{Page, PageAllocator};
use crate::sync::{Mutex, OnceLock};
use crate::task::{cpu_context_switch, CpuContext, InterruptMask};

/// An auto-incrementing task ID
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

/// A representation of a task's parent task ID
#[derive(Debug)]
pub enum ParentTaskId {
    Root,
    TaskId(TaskId),
}

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

/// A round-robin task scheduler
#[derive(Debug)]
pub struct Scheduler {
    tasks: UnsafeCell<BTreeMap<TaskId, Box<Task>>>,
    queue: UnsafeCell<VecDeque<TaskId>>,
    current_task: UnsafeCell<Vec<Option<TaskId>>>,
    reset_timer: OnceLock<fn()>,
}

impl Scheduler {
    pub fn instance() -> &'static Self {
        static INSTANCE: Scheduler = Scheduler::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            tasks: UnsafeCell::new(BTreeMap::new()),
            queue: UnsafeCell::new(VecDeque::new()),
            current_task: UnsafeCell::new(Vec::new()),
            reset_timer: OnceLock::new(),
        }
    }

    pub fn set_num_cores(&self, num_cores: usize) {
        let current_task = self.current_task.get();

        InterruptMask::instance().call(|| unsafe {
            let current_len = (*current_task).len();
            (*current_task).extend((current_len..num_cores).map(|_| None));
        });
    }

    pub fn set_reset_timer(&self, reset_timer: fn()) {
        self.reset_timer.set(reset_timer).unwrap();
    }

    pub fn create_and_become_init(&self) -> TaskId {
        let tasks = self.tasks.get();
        let queue = self.queue.get();
        let current_task = self.current_task.get();
        let core_num = self.current_core();

        let id = TaskId::next();
        let task = Task::new(id, ParentTaskId::Root, "init".to_owned());

        InterruptMask::instance().call(|| unsafe {
            assert!((*tasks).is_empty());
            assert!((*queue).is_empty());
            assert!(core_num == 0);

            (*tasks).insert(id, Box::new(task));

            // Skip the queue and set as current task.
            (*current_task)[core_num] = Some(id);
        });

        id
    }

    pub fn create_kthread(&self, func: fn()) -> TaskId {
        let tasks = self.tasks.get();
        let queue = self.queue.get();

        let id = TaskId::next();
        let mut task = Task::new(id, ParentTaskId::Root, "kthread".to_owned());

        unsafe {
            task.cpu_context.set_program_counter(func as *const u64);

            let page = PageAllocator::instance().alloc();
            task.cpu_context
                .set_stack_pointer(page.end_exclusive() as *mut u64);
            task.pages.push(page);
        }

        InterruptMask::instance().call(|| unsafe {
            (*tasks).insert(id, Box::new(task));
            (*queue).push_back(id);
        });

        id
    }

    pub unsafe fn schedule(&self) {
        let tasks = self.tasks.get();
        let queue = self.queue.get();
        let current_task = self.current_task.get();
        let core_num = self.current_core();

        InterruptMask::instance().lock();

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
            scheduler_after_schedule,
        );
    }

    unsafe fn after_schedule(&self) {
        InterruptMask::instance().unlock();

        (self.reset_timer.get().unwrap())();
    }

    // TODO: Only one core is currently supported.
    fn current_core(&self) -> usize {
        0
    }
}

#[no_mangle]
unsafe extern "C" fn scheduler_after_schedule() {
    Scheduler::instance().after_schedule();
}

unsafe impl Send for Scheduler {}
unsafe impl Sync for Scheduler {}
