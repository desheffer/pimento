use crate::context::TaskExecutionService;
use crate::fs::PathInfo;
use crate::println;

/// An encapsulation of the core functionality of the kernel.
pub struct Kernel<'a> {
    task_execution: &'a TaskExecutionService,
}

impl<'a> Kernel<'a> {
    /// Creates a kernel.
    pub fn new(task_execution: &'a TaskExecutionService) -> Self {
        Self { task_execution }
    }

    /// Runs the kernel after the system is initialized.
    pub fn run(&self) -> Result<(), ()> {
        println!("Hello, world!");

        let cli_path = PathInfo::absolute("/bin/cli").unwrap();
        self.task_execution.execute(&cli_path)?;

        Ok(())
    }
}
