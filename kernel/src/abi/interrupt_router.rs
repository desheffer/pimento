use core::mem::transmute;

use alloc::vec::Vec;

use crate::device::InterruptController;
use crate::sync::{Arc, Mutex};

type HandlerFn = unsafe fn(HandlerData);
type HandlerData = *const ();

/// A generic interrupt router.
///
/// When an interrupt is raised, the interrupt router finds the source of the interrupt and calls
/// the handler registered to it.
pub struct InterruptRouter {
    interrupts: Mutex<Vec<Interrupt>>,
}

impl InterruptRouter {
    /// Creates an interrupt router.
    pub fn new() -> Self {
        Self {
            interrupts: Mutex::new(Vec::new()),
        }
    }

    /// Enables a specific interrupt provided by an interrupt controller and registers a function
    /// to handle it.
    pub unsafe fn enable<T>(
        &self,
        interrupt_controller: Arc<dyn InterruptController>,
        number: u64,
        handler_func: unsafe fn(&T),
        handler_data: &'static T,
    ) {
        let mut interrupts = self.interrupts.lock();
        interrupts.push(Interrupt {
            controller: interrupt_controller.clone(),
            number,
            handler_func: transmute::<unsafe fn(&T), unsafe fn(*const ())>(handler_func),
            handler_data: handler_data as *const _ as _,
        });
        interrupt_controller.enable(number);
    }

    /// Handles an interrupt after it has been detected.
    pub unsafe fn handle(&self) {
        let mut handler: Option<(HandlerFn, HandlerData)> = None;

        let interrupts = self.interrupts.lock();
        for interrupt in &*interrupts {
            if interrupt.controller.is_pending(interrupt.number) {
                interrupt.controller.clear(interrupt.number);
                handler = Some((interrupt.handler_func, interrupt.handler_data));
                break;
            }
        }

        // Some interrupts may trigger a context switch. This necessitates that all locks be
        // released before calling the handler function.
        drop(interrupts);

        if let Some(handler) = handler {
            (handler.0)(handler.1);
        }
    }
}

/// The values representing an interrupt that has been enabled.
struct Interrupt {
    controller: Arc<dyn InterruptController>,
    number: u64,
    handler_func: HandlerFn,
    handler_data: HandlerData,
}

unsafe impl Send for Interrupt {}
unsafe impl Sync for Interrupt {}
