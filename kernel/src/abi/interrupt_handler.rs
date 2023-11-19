use alloc::vec::Vec;

use crate::device::InterruptController;
use crate::sync::{Arc, Mutex};

/// A generic interrupt handler
///
/// When an interrupt is raised, the local interrupt handler will determine the source of the
/// interrupt, and then it will call the handler registered with that source.
pub struct LocalInterruptHandler {
    interrupts: Mutex<Vec<Interrupt>>,
}

impl LocalInterruptHandler {
    pub const fn new() -> Self {
        Self {
            interrupts: Mutex::new(Vec::new()),
        }
    }

    pub fn enable(
        &self,
        interrupt_controller: Arc<dyn InterruptController>,
        number: u64,
        handler: unsafe fn(*const ()),
        data: *const (),
    ) {
        let mut handlers = self.interrupts.lock();
        handlers.push(Interrupt {
            controller: interrupt_controller.clone(),
            number,
            handler,
            data,
        });
        interrupt_controller.enable(number);
    }

    pub unsafe fn handle(&self) {
        let mut handler: Option<unsafe fn(*const ())> = None;
        let mut data: Option<*const ()> = None;

        let interrupts = self.interrupts.lock();
        for interrupt in &*interrupts {
            if interrupt.controller.is_pending(interrupt.number) {
                interrupt.controller.clear(interrupt.number);
                handler = Some(interrupt.handler);
                data = Some(interrupt.data);
                break;
            }
        }
        drop(interrupts);

        if let Some(handler) = handler {
            if let Some(data) = data {
                (handler)(data);
            }
        }
    }
}

/// The values representing an interrupt that is enabled
struct Interrupt {
    controller: Arc<dyn InterruptController>,
    number: u64,
    handler: unsafe fn(*const ()),
    data: *const (),
}

unsafe impl Send for Interrupt {}
unsafe impl Sync for Interrupt {}
