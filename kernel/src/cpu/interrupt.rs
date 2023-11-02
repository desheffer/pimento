use alloc::vec::Vec;

use crate::sync::{Mutex, OnceLock};

pub trait Interrupt {
    fn enable(&self);

    fn pending(&self) -> bool;

    fn clear(&self);
}

pub struct InterruptHandler {
    handlers: Mutex<Vec<(&'static dyn Interrupt, fn())>>,
}

impl InterruptHandler {
    pub fn instance() -> &'static Self {
        static INSTANCE: OnceLock<InterruptHandler> = OnceLock::new();
        INSTANCE.get_or_init(|| Self::new())
    }

    fn new() -> Self {
        Self {
            handlers: Mutex::new(Vec::new()),
        }
    }

    pub fn enable(&self, interrupt: &'static dyn Interrupt, handler: fn()) {
        self.handlers.lock().push((interrupt, handler));
        interrupt.enable();
    }

    pub fn handle(&self) {
        for handler in &*self.handlers.lock() {
            if handler.0.pending() {
                handler.1();
                handler.0.clear();
                break;
            }
        }
    }
}
