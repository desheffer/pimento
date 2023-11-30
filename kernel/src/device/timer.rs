use core::time::Duration;

use crate::sync::{Arc, Mutex};

/// A timer for interrupting the currently running process.
pub trait Timer: Send + Sync {
    /// Sets the timer to expire after the given duration.
    fn set_duration(&self, duration: Duration);
}

/// A `Timer` implementation that is statically accessible.
pub struct TimerImpl {
    inner: Mutex<Option<Arc<dyn Timer>>>,
}

impl TimerImpl {
    /// Gets the timer implementation.
    pub fn instance() -> &'static Self {
        static INSTANCE: TimerImpl = TimerImpl::new();
        &INSTANCE
    }

    const fn new() -> Self {
        TimerImpl {
            inner: Mutex::new(None),
        }
    }

    /// Set the preferred timer instance.
    pub fn set_inner(&self, inner: Arc<dyn Timer>) {
        *self.inner.lock() = Some(inner);
    }
}

impl Timer for TimerImpl {
    fn set_duration(&self, duration: Duration) {
        self.inner
            .lock()
            .as_ref()
            .expect("TimerImpl::set_inner() was expected")
            .set_duration(duration)
    }
}
