use core::fmt::Debug;
use core::time::Duration;

/// A counter of time elapsed since boot
pub trait Counter: Debug + Send + Sync {
    fn uptime(&self) -> Duration;
}
