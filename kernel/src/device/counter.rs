use core::time::Duration;

/// A counter of time elapsed since boot
pub trait Counter: Sync {
    fn uptime(&self) -> Duration;
}
