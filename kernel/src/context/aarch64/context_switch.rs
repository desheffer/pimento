use core::mem::transmute;

use crate::context::{Scheduler, Task};
use crate::cpu::cpu_context_switch;
use crate::memory::memory_context_switch;

/// An AArch64 CPU context switcher.
pub struct ContextSwitch {}

impl ContextSwitch {
    /// Creates an AArch64 context switcher.
    pub const fn new() -> Self {
        Self {}
    }

    /// Performs a context switch.
    pub unsafe fn switch(
        &self,
        prev: &mut Task,
        next: &mut Task,
        after_func: unsafe extern "C" fn(&Scheduler),
        after_data: &Scheduler,
    ) {
        memory_context_switch(&mut next.memory_context);

        cpu_context_switch(
            &mut prev.cpu_context,
            &mut next.cpu_context,
            transmute(after_func),
            after_data as *const _ as *const _,
        );
    }
}
