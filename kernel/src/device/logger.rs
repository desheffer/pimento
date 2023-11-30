use crate::sync::{Arc, Mutex};

/// A logger for kernel output.
pub trait Logger: Send + Sync {
    /// Writes a string to the log.
    fn write_str(&self, s: &str);
}

/// A `Logger` implementation that is statically accessible.
pub struct LoggerImpl {
    inner: Mutex<Option<Arc<dyn Logger>>>,
}

impl LoggerImpl {
    /// Gets the logger implementation.
    pub fn instance() -> &'static Self {
        static INSTANCE: LoggerImpl = LoggerImpl::new();
        &INSTANCE
    }

    const fn new() -> Self {
        LoggerImpl {
            inner: Mutex::new(None),
        }
    }

    /// Set the preferred logger instance.
    pub fn set_inner(&self, inner: Arc<dyn Logger>) {
        *self.inner.lock() = Some(inner);
    }
}

impl Logger for LoggerImpl {
    fn write_str(&self, s: &str) {
        if let Some(logger) = self.inner.lock().as_ref() {
            logger.write_str(s)
        }
    }
}
