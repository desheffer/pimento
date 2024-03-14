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
    current_task: UnsafeCell<Vec<Option<TaskId>>>,
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
            current_task: UnsafeCell::new(vec![None; num_cores]),
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
            assert!(self.tasks().is_empty());
            assert!(self.queue().is_empty());

            let core_num = self.current_core();
            assert!(core_num == 0);

            self.tasks().insert(id, Box::new(task));

            // Skip the queue and set as current task.
            self.current_task()[core_num] = Some(id);
        });

        id
    }

    /// Adds a task to the scheduling queue.
    pub fn add_task(&self, task: Task) -> TaskId {
        let id = task.id;

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            self.tasks().insert(id, Box::new(task));
            self.queue().push_back(id);
        });

        id
    }

    /// Determines the next task to run and performs a context switch.
    pub unsafe fn schedule(&self) {
        let core_num = self.current_core();

        self.lock.lock();

        // Put the current task back into the queue and get the next task.
        let prev_task_id = self.current_task()[core_num].unwrap();
        self.queue().push_back(prev_task_id);
        let next_task_id = self.queue().pop_front().unwrap();
        self.current_task()[core_num] = Some(next_task_id);

        let prev_task = self.tasks().get_mut(&prev_task_id).unwrap();
        let next_task = self.tasks().get_mut(&next_task_id).unwrap();

        /// Wraps the `after_schedule` function so that it can be called after the context switch.
        unsafe extern "C" fn after_schedule_trampoline(scheduler: &Scheduler) {
            scheduler.after_schedule();
        }

        // SAFETY: Tasks are passed as immutable references. This is safe as long as all access is
        // gated by the exclusive lock.
        self.context_switch
            .switch(prev_task, next_task, after_schedule_trampoline, self);

        // The previous function call might not return or might return in a different context. All
        // clean up should be handled before that call or in the `after_schedule` function.
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

    #[allow(clippy::mut_from_ref)]
    unsafe fn tasks(&self) -> &mut BTreeMap<TaskId, Box<Task>> {
        &mut *self.tasks.get()
    }

    #[allow(clippy::mut_from_ref)]
    unsafe fn queue(&self) -> &mut VecDeque<TaskId> {
        &mut *self.queue.get()
    }

    #[allow(clippy::mut_from_ref)]
    unsafe fn current_task(&self) -> &mut Vec<Option<TaskId>> {
        &mut *self.current_task.get()
    }
}

unsafe impl Send for Scheduler {}
unsafe impl Sync for Scheduler {}
