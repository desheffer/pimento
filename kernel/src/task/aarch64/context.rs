use core::mem::transmute;

use crate::cpu::cpu_context_switch;
use crate::memory::memory_context_switch;
use crate::task::{Scheduler, Task};

/// Performs a context switch.
pub unsafe fn context_switch(
    prev: &Task,
    next: &Task,
    after_func: unsafe extern "C" fn(&Scheduler),
    after_data: &Scheduler,
) {
    memory_context_switch(&next.memory_context);

    let after_func =
        transmute::<unsafe extern "C" fn(&Scheduler), unsafe extern "C" fn(*const ())>(after_func);

    cpu_context_switch(
        prev.cpu_context.registers.get(),
        next.cpu_context.registers.get(),
        after_func,
        after_data as *const _ as _,
    );
}
