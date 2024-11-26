use core::cell::UnsafeCell;
use core::time::Duration;

use alloc::collections::{BTreeMap, VecDeque};
use alloc::sync::Arc;
use alloc::vec;
use alloc::vec::Vec;

use crate::cpu::{critical, current_core, enable_interrupts};
use crate::device::Timer;
use crate::sync::{Lock, OnceLock};
use crate::task::{context_switch, Task, TaskId};

static SCHEDULER: OnceLock<&'static Scheduler> = OnceLock::new();

/// A round-robin task scheduler.
pub struct Scheduler {
    lock: Lock,
    tasks: UnsafeCell<BTreeMap<TaskId, Arc<Task>>>,
    queue: UnsafeCell<VecDeque<TaskId>>,
    current_tasks: UnsafeCell<Vec<Option<TaskId>>>,
    timer: Arc<dyn Timer>,
    quantum: Duration,
}

impl Scheduler {
    /// Creates a scheduler.
    pub fn new(num_cores: usize, timer: Arc<dyn Timer>, quantum: Duration) -> Self {
        Self {
            lock: Lock::new(),
            tasks: UnsafeCell::new(BTreeMap::new()),
            queue: UnsafeCell::new(VecDeque::new()),
            current_tasks: UnsafeCell::new(vec![None; num_cores]),
            timer,
            quantum,
        }
    }

    /// Assumes the role of the kernel initialization task.
    pub unsafe fn become_kinit(&self, task: Task) -> Result<TaskId, ()> {
        let task_id = task.id;

        // SAFETY: Safe because call is behind a lock.
        critical(|| unsafe {
            self.lock.call(|| {
                let tasks = &mut *self.tasks.get();
                assert!(tasks.is_empty());
                tasks.insert(task_id, Arc::new(task));

                let current_tasks = &mut *self.current_tasks.get();
                let current_core = current_core();
                assert!(current_core == 0);
                assert!(current_tasks[current_core] == None);
                current_tasks[current_core] = Some(task_id);
            });
        });

        self.schedule();

        Ok(task_id)
    }

    /// Adds a task to the scheduling queue.
    pub fn add_task(&self, task: Task) -> Result<TaskId, ()> {
        let task_id = task.id;

        // SAFETY: Safe because call is behind a lock.
        critical(|| unsafe {
            self.lock.call(|| {
                let tasks = &mut *self.tasks.get();
                tasks.insert(task_id, Arc::new(task));

                let queue = &mut *self.queue.get();
                queue.push_back(task_id);
            });
        });

        Ok(task_id)
    }

    /// Gets a reference to the task with the given task ID.
    pub fn task(&self, task_id: &TaskId) -> Option<Arc<Task>> {
        // SAFETY: Safe because call is behind a lock.
        critical(|| unsafe {
            self.lock.call(|| {
                let tasks = &*self.tasks.get();
                tasks.get(&task_id).cloned()
            })
        })
    }

    /// Gets the current task ID.
    pub fn current_task_id(&self) -> TaskId {
        // SAFETY: Safe because call is behind a lock.
        critical(|| unsafe {
            self.lock.call(|| {
                let current_tasks = &*self.current_tasks.get();
                current_tasks[current_core()].unwrap()
            })
        })
    }

    /// Gets the currently running task.
    pub fn current_task(&self) -> Arc<Task> {
        self.task(&self.current_task_id()).unwrap()
    }

    /// Determines the next task to run and performs a context switch.
    pub unsafe fn schedule(&self) {
        let prev_task;
        let next_task;

        {
            let tasks = &*self.tasks.get();
            let queue = &mut *self.queue.get();
            let current_tasks = &mut *self.current_tasks.get();

            let current_core = current_core();

            self.lock.lock();

            // Put the current task back into the queue and get the next task.
            let prev_task_id = current_tasks[current_core].unwrap();
            queue.push_back(prev_task_id);
            let next_task_id = queue.pop_front().unwrap();
            current_tasks[current_core] = Some(next_task_id);

            prev_task = tasks.get(&prev_task_id).unwrap();
            next_task = tasks.get(&next_task_id).unwrap();
        }

        /// Wraps the `after_schedule` function so that it can be called after the context switch.
        unsafe extern "C" fn after_schedule_trampoline(scheduler: &Scheduler) {
            scheduler.after_schedule();
        }

        // SAFETY: Tasks are passed as immutable references. This is safe as long as the exclusive
        // lock is held.
        context_switch(prev_task, next_task, after_schedule_trampoline, self);

        // The previous `switch` function call will most likely return into a different context.
        // All clean up should be handled before that call or in the `after_schedule` function.
    }

    /// Releases the lock and cleans up after the `schedule` function.
    unsafe fn after_schedule(&self) {
        self.lock.unlock();

        self.timer.set_duration(self.quantum);

        enable_interrupts();
    }
}

/// Sets the scheduler.
pub fn set_scheduler(scheduler: &'static Scheduler) -> Result<(), ()> {
    SCHEDULER.set(scheduler).map_err(|_| ())
}

/// Gets the currently running task.
pub fn current_task() -> Arc<Task> {
    SCHEDULER.get().unwrap().current_task().clone()
}
