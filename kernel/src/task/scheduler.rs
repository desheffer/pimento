use core::pin::Pin;

use alloc::borrow::ToOwned;
use alloc::boxed::Box;
use alloc::collections::{BTreeMap, VecDeque};
use alloc::string::String;
use alloc::vec::Vec;

use crate::memory::{Page, PageAllocator};
use crate::sync::{Mutex, OnceLock};
use crate::task::{cpu_switch, CpuContext, InterruptMask};

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
    tasks: Mutex<BTreeMap<TaskId, Pin<Box<Task>>>>,
    queue: Mutex<VecDeque<TaskId>>,
    current_task: Mutex<Vec<Option<TaskId>>>,
    after_schedule: OnceLock<fn()>,
}

impl Scheduler {
    pub fn instance() -> &'static Self {
        static INSTANCE: Scheduler = Scheduler::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            tasks: Mutex::new(BTreeMap::new()),
            queue: Mutex::new(VecDeque::new()),
            current_task: Mutex::new(Vec::new()),
            after_schedule: OnceLock::new(),
        }
    }

    // TODO: Currently only 1 core is supported.
    pub fn set_num_cores(&self, num_cores: usize) {
        let mut current_task = self.current_task.lock();
        let current_len = current_task.len();
        current_task.extend((current_len..num_cores).map(|_| None));
    }

    pub fn create_and_become_init(&self) -> TaskId {
        assert!(self.tasks.lock().is_empty());
        assert!(self.queue.lock().is_empty());

        let id = TaskId::next();
        let task = Task::new(id, ParentTaskId::Root, "init".to_owned());

        InterruptMask::instance().call(|| {
            self.tasks.lock().insert(id, Box::pin(task));

            // Skip the queue and set as current task.
            self.current_task.lock()[0] = Some(id);
        });

        id
    }

    pub fn create_kthread(&self, func: fn()) -> TaskId {
        let id = TaskId::next();
        let mut task = Task::new(id, ParentTaskId::Root, "kthread".to_owned());

        unsafe {
            task.cpu_context.set_program_counter(func as *const u64);

            let page = PageAllocator::instance().alloc();
            task.cpu_context
                .set_stack_pointer(page.end_exclusive() as *mut u64);
            task.pages.push(page);
        }

        InterruptMask::instance().call(|| {
            self.tasks.lock().insert(id, Box::pin(task));
            self.queue.lock().push_back(id);
        });

        id
    }

    pub unsafe fn schedule(&self) {
        InterruptMask::instance().lock();

        let tasks = self.tasks.lock();
        let mut queue = self.queue.lock();
        let mut current_task = self.current_task.lock();

        // Put the current task back into the queue and get the next task.
        let old_task_id = current_task[0].unwrap();
        queue.push_back(old_task_id);
        let new_task_id = queue.pop_front().unwrap();
        current_task[0] = Some(new_task_id);

        let old_task = tasks.get(&old_task_id).unwrap();
        let new_task = tasks.get(&new_task_id).unwrap();

        let old_cpu_context = &old_task.cpu_context as *const CpuContext as *mut CpuContext;
        let new_cpu_context = &new_task.cpu_context as *const CpuContext as *mut CpuContext;

        drop(current_task);
        drop(queue);
        drop(tasks);

        InterruptMask::instance().unlock();

        (self.after_schedule.get().unwrap())();

        cpu_switch(old_cpu_context, new_cpu_context);
    }

    pub fn set_after_schedule(&self, after_schedule: fn()) {
        self.after_schedule.set(after_schedule).unwrap();
    }
}
