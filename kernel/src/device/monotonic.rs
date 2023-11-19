use core::fmt::Debug;
use core::time::Duration;

/// A monotonic clock.
pub trait Monotonic: Debug + Send + Sync {
    fn monotonic(&self) -> Duration;
}
