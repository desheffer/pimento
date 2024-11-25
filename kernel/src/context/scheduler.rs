use core::cell::UnsafeCell;
use core::time::Duration;

use alloc::collections::{BTreeMap, VecDeque};
use alloc::sync::Arc;
use alloc::vec;
use alloc::vec::Vec;

use crate::context::{ContextSwitch, Task, TaskId};
use crate::cpu::{critical, enable_interrupts};
use crate::device::Timer;
use crate::sync::Lock;

/// A round-robin task scheduler.
pub struct Scheduler {
    lock: Lock,
    tasks: UnsafeCell<BTreeMap<TaskId, Arc<Task>>>,
    queue: UnsafeCell<VecDeque<TaskId>>,
    current_tasks: UnsafeCell<Vec<Option<TaskId>>>,
    timer: Arc<dyn Timer>,
    quantum: Duration,
    context_switch: &'static ContextSwitch,
}

impl Scheduler {
    /// Creates a scheduler.
    pub fn new(
        num_cores: usize,
        timer: Arc<dyn Timer>,
        quantum: Duration,
        context_switch: &'static ContextSwitch,
    ) -> Self {
        Self {
            lock: Lock::new(),
            tasks: UnsafeCell::new(BTreeMap::new()),
            queue: UnsafeCell::new(VecDeque::new()),
            current_tasks: UnsafeCell::new(vec![None; num_cores]),
            timer,
            quantum,
            context_switch,
        }
    }

    /// Assumes the role of the kernel initialization task.
    pub fn become_kinit(&self, task: Task) -> Result<TaskId, ()> {
        let id = task.id;

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            let tasks = &mut *self.tasks.get();
            let queue = &mut *self.queue.get();
            let current_tasks = &mut *self.current_tasks.get();

            let core_num = self.current_core();

            if !tasks.is_empty() || !queue.is_empty() || core_num != 0 {
                return Err(());
            }

            tasks.insert(id, Arc::new(task));

            // Skip the queue and set as current task.
            current_tasks[core_num] = Some(id);

            Ok(id)
        })
    }

    /// Adds a task to the scheduling queue.
    pub fn add_task(&self, task: Task) -> Result<TaskId, ()> {
        let id = task.id;

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            let tasks = &mut *self.tasks.get();
            let queue = &mut *self.queue.get();

            tasks.insert(id, Arc::new(task));
            queue.push_back(id);

            Ok(id)
        })
    }

    /// Determines the next task to run and performs a context switch.
    pub unsafe fn schedule(&self) {
        let tasks = &*self.tasks.get();
        let queue = &mut *self.queue.get();
        let current_tasks = &mut *self.current_tasks.get();

        let core_num = self.current_core();

        self.lock.lock();

        // Put the current task back into the queue and get the next task.
        let prev_task_id = current_tasks[core_num].unwrap();
        queue.push_back(prev_task_id);
        let next_task_id = queue.pop_front().unwrap();
        current_tasks[core_num] = Some(next_task_id);

        let prev_task = tasks.get(&prev_task_id).unwrap();
        let next_task = tasks.get(&next_task_id).unwrap();

        /// Wraps the `after_schedule` function so that it can be called after the context switch.
        unsafe extern "C" fn after_schedule_trampoline(scheduler: &Scheduler) {
            scheduler.after_schedule();
        }

        // SAFETY: Tasks are passed as immutable references. This is safe as long as the exclusive
        // lock is held.
        self.context_switch
            .switch(prev_task, next_task, after_schedule_trampoline, self);

        // The previous `switch` function call will most likely return into a different context.
        // All clean up should be handled before that call or in the `after_schedule` function.
    }

    /// Releases the lock and cleans up after the `schedule` function.
    unsafe fn after_schedule(&self) {
        self.lock.unlock();

        self.timer.set_duration(self.quantum);

        enable_interrupts();
    }

    /// Gets the index number of the current CPU core.
    fn current_core(&self) -> usize {
        // TODO: Update to support multiple cores.
        0
    }

    /// Gets the current task ID.
    pub fn current_task_id(&self) -> TaskId {
        // SAFETY: Safe because call is behind a lock.
        critical(|| unsafe {
            self.lock.call(|| {
                let current_core = self.current_core();
                let current_tasks = &*self.current_tasks.get();

                current_tasks[current_core].unwrap()
            })
        })
    }

    /// Gets a reference to the task with the given task ID.
    pub fn task(&self, task_id: TaskId) -> Option<Arc<Task>> {
        // SAFETY: Safe because call is behind a lock.
        critical(|| unsafe {
            self.lock.call(|| {
                let tasks = &*self.tasks.get();

                tasks.get(&task_id).cloned()
            })
        })
    }
}

unsafe impl Send for Scheduler {}
unsafe impl Sync for Scheduler {}
