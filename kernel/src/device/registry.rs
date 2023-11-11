use crate::device::{Counter, Logger, Timer};
use crate::sync::Mutex;

/// A registry for storing concrete instances of generic devices
pub struct Registry {
    counter: Mutex<Option<&'static dyn Counter>>,
    logger: Mutex<Option<&'static dyn Logger>>,
    timer: Mutex<Option<&'static dyn Timer>>,
}

impl Registry {
    pub fn instance() -> &'static Self {
        static INSTANCE: Registry = Registry::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            counter: Mutex::new(None),
            logger: Mutex::new(None),
            timer: Mutex::new(None),
        }
    }

    pub fn set_counter(&self, counter: &'static dyn Counter) {
        *self.counter.lock() = Some(counter);
    }

    pub fn counter(&self) -> Option<&'static dyn Counter> {
        *self.counter.lock()
    }

    pub fn set_logger(&self, logger: &'static dyn Logger) {
        *self.logger.lock() = Some(logger);
    }

    pub fn logger(&self) -> Option<&'static dyn Logger> {
        *self.logger.lock()
    }

    pub fn set_timer(&self, timer: &'static dyn Timer) {
        *self.timer.lock() = Some(timer);
    }

    pub fn timer(&self) -> Option<&'static dyn Timer> {
        *self.timer.lock()
    }
}
