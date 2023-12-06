use core::cell::UnsafeCell;
use core::time::Duration;

use alloc::boxed::Box;
use alloc::collections::{BTreeMap, VecDeque};
use alloc::vec::Vec;

use crate::context::{ContextSwitcher, Task, TaskId};
use crate::cpu::InterruptMask;
use crate::device::{Timer, TimerImpl};
use crate::memory::PageAllocator;
use crate::sync::{Mutex, OnceLock, UninterruptibleLock};

/// A round-robin task scheduler.
pub struct Scheduler<'a> {
    lock: UninterruptibleLock,
    tasks: UnsafeCell<BTreeMap<TaskId, Box<Task>>>,
    queue: UnsafeCell<VecDeque<TaskId>>,
    current_task: UnsafeCell<Vec<Option<TaskId>>>,
    timer: &'a TimerImpl,
    quantum: Duration,
    page_allocator: &'a PageAllocator,
    context_switcher: &'a ContextSwitcher,
}

static INSTANCE: OnceLock<Scheduler> = OnceLock::new();
static INIT_NUM_CORES: Mutex<Option<usize>> = Mutex::new(None);
static INIT_QUANTUM: Mutex<Option<Duration>> = Mutex::new(None);

impl<'a> Scheduler<'a> {
    /// Sets the number of cores for the system.
    pub fn set_num_cores(num_cores: usize) {
        assert!(!INSTANCE.is_initialized());
        *INIT_NUM_CORES.lock() = Some(num_cores);
    }

    /// Sets the quantum (time slot size) as a duration of time.
    pub fn set_quantum(quantum: Duration) {
        assert!(!INSTANCE.is_initialized());
        *INIT_QUANTUM.lock() = Some(quantum);
    }

    /// Gets or initializes the scheduler.
    pub fn instance() -> &'static Self {
        INSTANCE.get_or_init(|| {
            let num_cores = INIT_NUM_CORES
                .lock()
                .take()
                .expect("Scheduler::set_num_cores() was expected");
            let quantum = INIT_QUANTUM
                .lock()
                .take()
                .expect("Scheduler::set_quantum() was expected");

            Self::new(
                num_cores,
                TimerImpl::instance(),
                quantum,
                PageAllocator::instance(),
                ContextSwitcher::instance(),
            )
        })
    }

    fn new(
        num_cores: usize,
        timer: &'a TimerImpl,
        quantum: Duration,
        page_allocator: &'a PageAllocator,
        context_switcher: &'a ContextSwitcher,
    ) -> Self {
        let mut current_task = Vec::new();
        current_task.extend((0..num_cores).map(|_| None));

        Self {
            lock: UninterruptibleLock::new(),
            tasks: UnsafeCell::new(BTreeMap::new()),
            queue: UnsafeCell::new(VecDeque::new()),
            current_task: UnsafeCell::new(current_task),
            timer,
            quantum,
            page_allocator,
            context_switcher,
        }
    }

    /// Assumes the role of the kernel initialization task.
    pub fn become_kinit(&self, task: Task) {
        let id = task.id;
        let core_num = self.current_core();

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            assert!(self.tasks().is_empty());
            assert!(self.queue().is_empty());
            assert!(core_num == 0);

            self.tasks().insert(id, Box::new(task));

            // Skip the queue and set as current task.
            self.current_task()[core_num] = Some(id);
        });
    }

    /// Adds a task to the scheduling queue.
    pub fn add_task(&self, task: Task) {
        let id = task.id;

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            self.tasks().insert(id, Box::new(task));
            self.queue().push_back(id);
        });
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

        // SAFETY: The `cpu_context` objects are passed as immutable references. This is safe as
        // long as all access is gated by the exclusive lock.
        self.context_switcher
            .switch(prev_task, next_task, _scheduler_after_schedule);

        // The previous function call might not return or might return in a different context. All
        // clean up should be handled before that call or in the `after_schedule` function.
    }

    /// Releases the lock and cleans up after the `schedule` function.
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

/// Wraps the `after_schedule` function so that it can be called after a context switch.
#[no_mangle]
unsafe extern "C" fn _scheduler_after_schedule() {
    Scheduler::instance().after_schedule()
}

unsafe impl Send for Scheduler<'_> {}
unsafe impl Sync for Scheduler<'_> {}
