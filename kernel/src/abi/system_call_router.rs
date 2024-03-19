use alloc::vec;
use alloc::vec::Vec;

use crate::abi::SystemCallNumber;
use crate::println;

/// A system call to be defined.
pub trait SystemCall {
    fn number(&self) -> SystemCallNumber;
    fn call(&self, _: usize, _: usize, _: usize, _: usize, _: usize, _: usize) -> isize;
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
        a: usize,
        b: usize,
        c: usize,
        d: usize,
        e: usize,
        f: usize,
    ) -> isize {
        let handler = self.handlers.get(num).unwrap_or(&None);

        if let Some(handler) = handler {
            handler.call(a, b, c, d, e, f)
        } else {
            println!("not implemented: system call {:#x}", num);
            -1
        }
    }
}
