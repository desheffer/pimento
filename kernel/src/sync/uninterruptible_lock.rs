use crate::cpu::InterruptMask;

/// A lock that cannot be interrupted while the lock is held.
///
/// This lock is designed for restricting access to critical sections of code. Interrupts are
/// disabled when the lock is held and restored (if previously enabled) when the lock is released.
///
/// This lock assumes that the system has a single core.
pub struct UninterruptibleLock {}

impl UninterruptibleLock {
    /// Creates an uninterruptible lock.
    pub const fn new() -> Self {
        Self {}
    }

    /// Disables interrupts and obtains the lock.
    pub fn lock(&self) {
        // If the system has a single core, then disabling interrupts is sufficient.
        // However, it might be useful to spin if the lock is held and interrupts are enabled.
        InterruptMask::instance().lock();
    }

    /// Releases the lock and enables interrupts.
    pub fn unlock(&self) {
        InterruptMask::instance().unlock();
    }

    /// Executes the given closure while holding the lock.
    pub fn call<F, R>(&self, f: F) -> R
    where
        F: FnOnce() -> R,
    {
        self.lock();

        let result = f();

        self.unlock();
        result
    }
}

unsafe impl Sync for UninterruptibleLock {}
