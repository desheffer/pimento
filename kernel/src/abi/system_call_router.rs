use alloc::vec::Vec;

use crate::abi::SYS;
use crate::sync::{Mutex, OnceLock};

type HandlerFn = unsafe fn(usize, usize, usize, usize, usize, usize) -> usize;

/// A generic system call router.
pub struct SystemCallRouter {
    handlers: Vec<Option<HandlerFn>>,
}

static INSTANCE: OnceLock<SystemCallRouter> = OnceLock::new();
static INIT_HANDLERS: Mutex<Vec<Option<HandlerFn>>> = Mutex::new(Vec::new());

impl SystemCallRouter {
    /// Registers a system call with the given number.
    pub fn register(sys: SYS, handler: HandlerFn) {
        assert!(INSTANCE.get().is_none());

        let mut handlers = INIT_HANDLERS.lock();
        let num = sys as usize;
        if num >= handlers.len() {
            handlers.resize(num + 1, None);
        }

        assert!(handlers[num].is_none());
        handlers[num] = Some(handler);
    }

    /// Gets the system call handler.
    pub fn instance() -> &'static Self {
        let handlers = INIT_HANDLERS.lock().clone();

        INSTANCE.get_or_init(|| Self::new(handlers))
    }

    fn new(handlers: Vec<Option<HandlerFn>>) -> Self {
        Self { handlers }
    }

    /// Handles a system call that has been requested.
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
