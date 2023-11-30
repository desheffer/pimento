use core::time::Duration;

use crate::sync::{Arc, Mutex};

/// A monotonic clock.
pub trait Monotonic: Send + Sync {
    /// Gets the system uptime.
    fn monotonic(&self) -> Duration;
}

/// A `Monotonic` implementation that is statically accessible.
pub struct MonotonicImpl {
    inner: Mutex<Option<Arc<dyn Monotonic>>>,
}

impl MonotonicImpl {
    /// Gets the monotonic implementation.
    pub fn instance() -> &'static Self {
        static INSTANCE: MonotonicImpl = MonotonicImpl::new();
        &INSTANCE
    }

    const fn new() -> Self {
        MonotonicImpl {
            inner: Mutex::new(None),
        }
    }

    /// Set the preferred monotonic instance.
    pub fn set_inner(&self, inner: Arc<dyn Monotonic>) {
        *self.inner.lock() = Some(inner);
    }
}

impl Monotonic for MonotonicImpl {
    fn monotonic(&self) -> Duration {
        self.inner
            .lock()
            .as_ref()
            .expect("MonotonicImpl::set_inner() was expected")
            .monotonic()
    }
}
