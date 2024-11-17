use crate::sync::Mutex;

/// An atomic counter.
// TODO: Use an AtomicU64 once the MMU is fully enabled (using normal memory).
pub struct AtomicCounter {
    next: Mutex<u64>,
}

impl AtomicCounter {
    /// Creates an atomic counter.
    pub const fn new(initial: u64) -> Self {
        Self {
            next: Mutex::new(initial),
        }
    }

    /// Increments the counter by one and returns the previous value.
    pub fn inc(&self) -> u64 {
        let mut next = self.next.lock();
        let last = *next;
        *next += 1;
        last
    }
}
