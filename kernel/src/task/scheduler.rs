use core::cell::UnsafeCell;
use core::time::Duration;

use alloc::borrow::ToOwned;
use alloc::boxed::Box;
use alloc::collections::{BTreeMap, VecDeque};
use alloc::string::String;
use alloc::vec::Vec;

use crate::device::{Timer, TimerImpl};
use crate::memory::{Page, PageAllocator};
use crate::sync::{Mutex, OnceLock, UninterruptibleLock};
use crate::task::{cpu_context_switch, CpuContext};

use super::InterruptMask;

/// An auto-incrementing task ID.
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
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
pub enum ParentTaskId {
    Root,
    TaskId(TaskId),
}

/// A task and the context it needs to run.
pub struct Task {
    id: TaskId,
    parent_id: ParentTaskId,
    name: String,
    stack: Option<Page>,
    cpu_context: CpuContext,
}

impl Task {
    fn new(
        id: TaskId,
        parent_id: ParentTaskId,
        name: String,
        stack: Option<Page>,
        cpu_context: CpuContext,
    ) -> Self {
        Task {
            id,
            parent_id,
            name,
            stack,
            cpu_context,
        }
    }
}

/// A round-robin task scheduler.
pub struct Scheduler<'a> {
    lock: UninterruptibleLock,
    tasks: UnsafeCell<BTreeMap<TaskId, Box<Task>>>,
    queue: UnsafeCell<VecDeque<TaskId>>,
    current_task: UnsafeCell<Vec<Option<TaskId>>>,
    timer: &'a TimerImpl,
    quantum: Duration,
    page_allocator: &'a PageAllocator,
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
            )
        })
    }

    fn new(
        num_cores: usize,
        timer: &'a TimerImpl,
        quantum: Duration,
        page_allocator: &'a PageAllocator,
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
        }
    }

    /// Creates a task for the kernel "init" process, which is presumed to be the currently running
    /// process. This is required after initialization.
    pub fn create_and_become_kinit(&self) -> TaskId {
        let core_num = self.current_core();

        let id = TaskId::next();
        let cpu_context = CpuContext::new();
        let task = Task::new(
            id,
            ParentTaskId::Root,
            "kinit".to_owned(),
            None,
            cpu_context,
        );

        // SAFETY: Safe because call is behind a lock.
        self.lock.call(|| unsafe {
            assert!(self.tasks().is_empty());
            assert!(self.queue().is_empty());
            assert!(core_num == 0);

            self.tasks().insert(id, Box::new(task));

            // Skip the queue and set as current task.
            self.current_task()[core_num] = Some(id);
        });

        id
    }

    /// Spawns a new kernel thread that will execute the given function.
    pub fn create_kthread(&self, func: fn()) -> TaskId {
        // SAFETY: Safe because the page is reserved.
        let page;
        let cpu_context;
        unsafe {
            page = self.page_allocator.alloc();
            cpu_context = CpuContext::new_with_task_entry(func, page.end_exclusive());
        }

        let id = TaskId::next();
        let task = Task::new(
            id,
            ParentTaskId::Root,
            "kthread".to_owned(),
            Some(page),
            cpu_context,
        );

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

        let prev_task = self.tasks().get(&prev_task_id).unwrap();
        let next_task = self.tasks().get(&next_task_id).unwrap();

        // SAFETY: The `cpu_context` objects are passed as immutable references. This is safe as
        // long as all access is gated by the exclusive lock.
        cpu_context_switch(
            &prev_task.cpu_context,
            &next_task.cpu_context,
            _scheduler_after_schedule,
        );

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

/// Wraps the `schedule` function so that it can be called from the interrupt handler.
#[no_mangle]
pub unsafe fn _scheduler_schedule() {
    Scheduler::instance().schedule()
}

/// Wraps the `after_schedule` function so that it can be called from the context switch function.
#[no_mangle]
unsafe extern "C" fn _scheduler_after_schedule() {
    Scheduler::instance().after_schedule()
}

unsafe impl Send for Scheduler<'_> {}
unsafe impl Sync for Scheduler<'_> {}
