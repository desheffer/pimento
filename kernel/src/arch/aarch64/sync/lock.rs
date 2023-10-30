use core::cell::UnsafeCell;

/// A simple lock
///
/// This lock assumes that the system has a single core and that interrupts are not enabled.
#[derive(Debug)]
pub struct Lock {
    locked: UnsafeCell<bool>,
}

impl Lock {
    pub const fn new() -> Self {
        Self {
            locked: UnsafeCell::new(false),
        }
    }

    pub fn lock(&self) {
        // SAFETY: Only safe on a single core when interrupts are not enabled.
        unsafe {
            if *self.locked.get() {
                panic!("deadlocked");
            }

            *self.locked.get() = true;
        }
    }

    pub fn unlock(&self) {
        // SAFETY: Only safe on a single core when interrupts are not enabled.
        unsafe {
            *self.locked.get() = false;
        }
    }

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

unsafe impl Sync for Lock {}
