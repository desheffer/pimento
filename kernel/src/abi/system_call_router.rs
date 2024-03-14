use alloc::vec::Vec;

pub type HandlerFn = unsafe fn(usize, usize, usize, usize, usize, usize) -> usize;

/// A generic system call router.
pub struct SystemCallRouter {
    handlers: Vec<Option<HandlerFn>>,
}

impl SystemCallRouter {
    /// Creates a system call router.
    pub fn new(handlers: Vec<Option<HandlerFn>>) -> Self {
        Self { handlers }
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
    ) -> usize {
        let handler = self.handlers.get(num).unwrap_or(&None);

        if let Some(handler) = handler {
            handler(a, b, c, d, e, f)
        } else {
            unimplemented!("system call ({:#x})", num)
        }
    }
}
