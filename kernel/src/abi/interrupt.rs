use alloc::vec::Vec;

use crate::sync::Mutex;

/// An interrupt that can be raised and detected
pub trait Interrupt: Sync {
    fn enable(&self);

    fn is_pending(&self) -> bool;

    fn clear(&self);
}

/// A generic interrupt handler
///
/// When an interrupt is raised, the local interrupt handler will determine the source of the
/// interrupt, and then it will call the handler registered with that source.
pub struct LocalInterruptHandler<'a> {
    handlers: Mutex<Vec<InterruptHandler<'a>>>,
}

impl<'a> LocalInterruptHandler<'a> {
    pub fn instance() -> &'static Self {
        static INSTANCE: LocalInterruptHandler = LocalInterruptHandler::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            handlers: Mutex::new(Vec::new()),
        }
    }

    pub fn enable(&self, interrupt: &'a dyn Interrupt, handler: fn()) {
        let mut handlers = self.handlers.lock();
        handlers.push(InterruptHandler(interrupt, handler));
        interrupt.enable();
    }

    pub fn handle(&self) {
        let mut func: Option<fn()> = None;

        let handlers = self.handlers.lock();
        for handler in &*handlers {
            if handler.0.is_pending() {
                handler.0.clear();
                func = Some(handler.1);
                break;
            }
        }
        drop(handlers);

        if let Some(func) = func {
            func();
        }
    }
}

/// An interrupt handler tied to a specific interrupt
struct InterruptHandler<'a>(&'a dyn Interrupt, fn());
