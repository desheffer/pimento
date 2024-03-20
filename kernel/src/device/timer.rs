use core::time::Duration;

/// A timer for interrupting the currently running process.
pub trait Timer {
    /// Sets the timer to expire after the given duration.
    fn set_duration(&self, duration: Duration);
}
