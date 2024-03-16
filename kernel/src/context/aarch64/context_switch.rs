use core::mem::transmute;

use crate::context::{ContextSwitch, Scheduler, Task};
use crate::cpu::cpu_context_switch;
use crate::memory::memory_context_switch;

/// An AArch64 CPU context switcher.
pub struct AArch64ContextSwitch {}

impl AArch64ContextSwitch {
    /// Creates an AArch64 context switcher.
    pub const fn new() -> Self {
        Self {}
    }
}

impl ContextSwitch for AArch64ContextSwitch {
    /// Performs a context switch.
    unsafe fn switch(
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
