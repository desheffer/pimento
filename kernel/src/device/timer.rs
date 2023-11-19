use core::fmt::Debug;
use core::time::Duration;

/// A timer for interrupting the currently running process
pub trait Timer: Debug + Send + Sync {
    fn set_duration(&self, duration: Duration);
}
