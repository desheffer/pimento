use crate::context::{Scheduler, Task};

pub trait ContextSwitch {
    /// Performs a context switch.
    unsafe fn switch(
        &self,
        prev: &mut Task,
        next: &mut Task,
        after_func: unsafe extern "C" fn(&Scheduler),
        after_data: &Scheduler,
    );
}
