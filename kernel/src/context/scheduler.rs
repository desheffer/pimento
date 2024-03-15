use core::cell::UnsafeCell;
use core::time::Duration;

use alloc::boxed::Box;
use alloc::collections::{BTreeMap, VecDeque};
use alloc::vec;
use alloc::vec::Vec;

use crate::context::{ContextSwitch, Task, TaskId};
use crate::cpu::INTERRUPT_MASK;
use crate::device::Timer;
use crate::sync::{Arc, UninterruptibleLock};

/// A round-robin task scheduler.
pub struct Scheduler {
    lock: UninterruptibleLock,
    tasks: UnsafeCell<BTreeMap<TaskId, Box<Task>>>,
    queue: UnsafeCell<VecDeque<TaskId>>,
    current_tasks: UnsafeCell<Vec<Option<TaskId>>>,
    timer: Arc<dyn Timer>,
    quantum: Duration,
    context_switch: &'static dyn ContextSwitch,
}

impl Scheduler {
    /// Creates a scheduler.
    pub fn new(
        num_cores: usize,
        timer: Arc<dyn Timer>,
        quantum: Duration,
        context_switch: &'static dyn ContextSwitch,
    ) -> Self {
        Self {
            lock: UninterruptibleLock::new(),
            tasks: UnsafeCell::new(BTreeMap::new()),
            queue: UnsafeCell::new(VecDeque::new()),
            current_tasks: UnsafeCell::new(vec![None; num_cores]),
            timer,
            quantum,
            context_switch,
        }
    }

    /// Assumes the role of the kernel initialization task.
    pub fn become_kinit(&self, task: Task) -> TaskId {
        let id = task.id;

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            let tasks = &mut *self.tasks.get();
            let queue = &mut *self.queue.get();
            let current_tasks = &mut *self.current_tasks.get();

            assert!(tasks.is_empty());
            assert!(queue.is_empty());

            let core_num = self.current_core();
            assert!(core_num == 0);

            tasks.insert(id, Box::new(task));

            // Skip the queue and set as current task.
            current_tasks[core_num] = Some(id);
        });

        id
    }

    /// Adds a task to the scheduling queue.
    pub fn add_task(&self, task: Task) -> TaskId {
        let id = task.id;

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            let tasks = &mut *self.tasks.get();
            let queue = &mut *self.queue.get();

            tasks.insert(id, Box::new(task));
            queue.push_back(id);
        });

        id
    }

    /// Determines the next task to run and performs a context switch.
    pub unsafe fn schedule(&self) {
        let core_num = self.current_core();
        let queue = &mut *self.queue.get();
        let current_tasks = &mut *self.current_tasks.get();

        self.lock.lock();

        // Put the current task back into the queue and get the next task.
        let prev_task_id = current_tasks[core_num].unwrap();
        queue.push_back(prev_task_id);
        let next_task_id = queue.pop_front().unwrap();
        current_tasks[core_num] = Some(next_task_id);

        let prev_task = (*self.tasks.get()).get_mut(&prev_task_id).unwrap();
        let next_task = (*self.tasks.get()).get_mut(&next_task_id).unwrap();

        /// Wraps the `after_schedule` function so that it can be called after the context switch.
        unsafe extern "C" fn after_schedule_trampoline(scheduler: &Scheduler) {
            scheduler.after_schedule();
        }

        // SAFETY: Tasks are passed as mutable references. This is safe as long as the exclusive
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

        INTERRUPT_MASK.enable_interrupts();
    }

    /// Gets the index number of the current CPU core.
    fn current_core(&self) -> usize {
        // TODO: Update to support multiple cores.
        0
    }
}

unsafe impl Send for Scheduler {}
unsafe impl Sync for Scheduler {}
