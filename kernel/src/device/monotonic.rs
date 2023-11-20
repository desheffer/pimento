use core::time::Duration;

/// A monotonic clock.
pub trait Monotonic: Send + Sync {
    fn monotonic(&self) -> Duration;
}
