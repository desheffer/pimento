use core::time::Duration;

/// A timer for interrupting the currently running process
pub trait Timer: Sync {
    fn set_duration(&self, duration: Duration);
}
