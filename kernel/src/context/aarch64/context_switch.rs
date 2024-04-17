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
        prev: &Task,
        next: &Task,
        after_func: unsafe extern "C" fn(&Scheduler),
        after_data: &Scheduler,
    ) {
        memory_context_switch(&next.memory_context);

        let after_func = transmute::<
            unsafe extern "C" fn(&Scheduler),
            unsafe extern "C" fn(*const ()),
        >(after_func);

        cpu_context_switch(
            prev.cpu_context.get(),
            next.cpu_context.get(),
            after_func,
            after_data as *const _ as _,
        );
    }
}
