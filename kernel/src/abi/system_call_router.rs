use alloc::vec;
use alloc::vec::Vec;

use crate::abi::{SystemCallError, SystemCallNumber};
use crate::println;

/// A system call to be defined.
pub trait SystemCall {
    /// Returns the system call number.
    fn number(&self) -> SystemCallNumber;

    /// Executes the system call.
    fn call(
        &self,
        _arg0: usize,
        _arg1: usize,
        _arg2: usize,
        _arg3: usize,
        _arg4: usize,
        _arg5: usize,
    ) -> Result<usize, SystemCallError>;
}

/// A generic system call router.
pub struct SystemCallRouter {
    handlers: Vec<Option<&'static dyn SystemCall>>,
}

impl SystemCallRouter {
    /// Creates a system call router.
    pub fn new(handlers: Vec<&'static dyn SystemCall>) -> Self {
        let max_number = handlers
            .iter()
            .map(|handler| handler.number() as usize)
            .max()
            .unwrap_or(0);

        let mut handlers_indexed = vec![None; max_number + 1];
        for handler in handlers {
            handlers_indexed[handler.number() as usize] = Some(handler);
        }

        Self {
            handlers: handlers_indexed,
        }
    }

    /// Handles a system call that has been requested.
    #[allow(clippy::too_many_arguments)]
    pub unsafe fn handle(
        &self,
        num: usize,
        arg0: usize,
        arg1: usize,
        arg2: usize,
        arg3: usize,
        arg4: usize,
        arg5: usize,
    ) -> isize {
        let handler = self.handlers.get(num).unwrap_or(&None);

        if let Some(handler) = handler {
            let res = handler.call(arg0, arg1, arg2, arg3, arg4, arg5);
            match res {
                Ok(val) => val as isize,
                Err(val) => -(val as isize),
            }
        } else {
            println!("not implemented: system call {:#x}", num);
            -1
        }
    }
}
