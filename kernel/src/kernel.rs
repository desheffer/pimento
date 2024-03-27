use alloc::borrow::ToOwned;

use crate::context::TaskExecutionService;
use crate::println;

/// An encapsulation of the core functionality of the kernel.
pub struct Kernel<'a> {
    task_execution: &'a TaskExecutionService,
}

impl<'a> Kernel<'a> {
    pub fn new(task_execution: &'a TaskExecutionService) -> Self {
        Self { task_execution }
    }

    /// Runs the kernel after the system is initialized.
    pub fn run(&self) -> Result<(), ()> {
        println!("Hello, world!");

        self.task_execution.execute("/bin/example".to_owned())?;

        Ok(())
    }
}
