use crate::context::Task;
use crate::cpu::cpu_context_switch;

/// AArch64 CPU context manager.
pub struct ContextSwitcher {}

impl ContextSwitcher {
    /// Gets the CPU context manager.
    pub fn instance() -> &'static Self {
        static INSTANCE: ContextSwitcher = ContextSwitcher::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {}
    }

    pub unsafe fn switch(&self, prev: &mut Task, next: &mut Task, after: unsafe extern "C" fn()) {
        cpu_context_switch(&mut prev.cpu_context, &mut next.cpu_context, after);
    }
}
