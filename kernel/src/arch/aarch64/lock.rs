use core::cell::UnsafeCell;

/// A simple lock
///
/// This lock assumes that the system has a single core and that interrupts are not enabled.
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
        unsafe {
            if *self.locked.get() {
                panic!("deadlocked");
            }

            *self.locked.get() = true;
        }
    }

    pub fn unlock(&self) {
        unsafe {
            *self.locked.get() = false;
        }
    }
}
