use alloc::string::String;
use alloc::vec;
use alloc::vec::Vec;

use crate::abi::{SystemCall, SystemCallError, SystemCallNumber};
use crate::context::Scheduler;
use crate::memory::UserVirtualAddress;

pub struct SysWrite {
    scheduler: &'static Scheduler,
}

impl SysWrite {
    /// Creates a write handler.
    pub fn new(scheduler: &'static Scheduler) -> Self {
        Self { scheduler }
    }
}

impl SystemCall for SysWrite {
    fn number(&self) -> SystemCallNumber {
        SystemCallNumber::Write
    }

    /// Writes a buffer to a file.
    ///
    /// This is a proof-of-concept and all output is written to the kernel logger.
    fn call(
        &self,
        arg0: usize,
        arg1: usize,
        arg2: usize,
        _arg3: usize,
        _arg4: usize,
        _arg5: usize,
    ) -> Result<usize, SystemCallError> {
        let fd = arg0;
        let buf = UserVirtualAddress::<u8>::new(arg1);
        let count = arg2;

        if fd != 1 {
            return Err(SystemCallError::BadFileNumber);
        }

        let task_id = self.scheduler.current_task_id();
        let task = self.scheduler.task(task_id).unwrap();

        let mut k_buf: Vec<u8> = vec![0; count];

        unsafe {
            task.memory_context
                .copy_from_user(buf, k_buf.as_mut_ptr(), count)
                .map_err(|_| SystemCallError::BadAddress)?;
        }

        crate::print!("{}", String::from_utf8_lossy(&k_buf));

        Ok(count)
    }
}
