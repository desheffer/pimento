use core::time::Duration;

/// A monotonic clock.
pub trait Monotonic {
    /// Gets the system uptime.
    fn monotonic(&self) -> Duration;
}
