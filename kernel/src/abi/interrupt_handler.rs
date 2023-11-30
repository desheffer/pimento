use alloc::vec::Vec;

use crate::device::InterruptController;
use crate::sync::{Arc, Mutex};

/// A generic interrupt handler.
///
/// When an interrupt is raised, the local interrupt handler will determine the source of the
/// interrupt, and then it will call the handler registered with that source.
pub struct LocalInterruptHandler {
    interrupts: Mutex<Vec<Interrupt>>,
}

impl LocalInterruptHandler {
    /// Gets the generic interrupt handler.
    pub fn instance() -> &'static Self {
        static INSTANCE: LocalInterruptHandler = LocalInterruptHandler::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            interrupts: Mutex::new(Vec::new()),
        }
    }

    /// Enables a specific interrupt provided by an interrupt controller and registers a function
    /// to handle it.
    pub fn enable(
        &self,
        interrupt_controller: Arc<dyn InterruptController>,
        number: u64,
        handler: unsafe fn(),
    ) {
        let mut handlers = self.interrupts.lock();
        handlers.push(Interrupt {
            controller: interrupt_controller.clone(),
            number,
            handler,
        });
        interrupt_controller.enable(number);
    }

    /// Handles an interrupt after it has been detected.
    pub unsafe fn handle(&self) {
        let mut handler: Option<unsafe fn()> = None;

        let interrupts = self.interrupts.lock();
        for interrupt in &*interrupts {
            if interrupt.controller.is_pending(interrupt.number) {
                interrupt.controller.clear(interrupt.number);
                handler = Some(interrupt.handler);
                break;
            }
        }
        drop(interrupts);

        if let Some(handler) = handler {
            (handler)();
        }
    }
}

/// The values representing an interrupt that has been enabled.
struct Interrupt {
    controller: Arc<dyn InterruptController>,
    number: u64,
    handler: unsafe fn(),
}

unsafe impl Send for Interrupt {}
unsafe impl Sync for Interrupt {}
