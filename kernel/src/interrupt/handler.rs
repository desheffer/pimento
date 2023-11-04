use alloc::vec::Vec;

use crate::sync::{Mutex, OnceLock};

/// An interrupt that can be raised and detected
pub trait Interrupt {
    fn enable(&self);

    fn is_pending(&self) -> bool;

    fn clear(&self);
}

/// A generic interrupt handler
///
/// When an interrupt is raised, the local interrupt handler will determine the source of the
/// interrupt, and then it will call the handler registered with that source.
pub struct LocalInterruptHandler {
    handlers: Mutex<Vec<InterruptHandler>>,
}

impl LocalInterruptHandler {
    pub fn instance() -> &'static Self {
        static INSTANCE: OnceLock<LocalInterruptHandler> = OnceLock::new();
        INSTANCE.get_or_init(|| Self::new())
    }

    fn new() -> Self {
        Self {
            handlers: Mutex::new(Vec::new()),
        }
    }

    pub fn enable(&self, interrupt: &'static dyn Interrupt, handler: fn()) {
        self.handlers.lock().push(InterruptHandler(interrupt, handler));
        interrupt.enable();
    }

    pub fn handle(&self) {
        for handler in &*self.handlers.lock() {
            if handler.0.is_pending() {
                handler.1();
                handler.0.clear();
                break;
            }
        }
    }
}

/// An interrupt handler tied to a specific interrupt.
struct InterruptHandler(&'static dyn Interrupt, fn());
