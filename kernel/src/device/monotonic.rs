use core::time::Duration;

/// A monotonic clock.
pub trait Monotonic: Send + Sync {
    /// Gets the system uptime.
    fn monotonic(&self) -> Duration;
}
