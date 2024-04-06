pub use self::arc::{Arc, Weak};
pub use self::arch::*;
pub use self::atomic_counter::AtomicCounter;
pub use self::mutex::Mutex;
pub use self::once::Once;
pub use self::once_lock::OnceLock;

mod arc;
mod atomic_counter;
mod mutex;
mod once;
mod once_lock;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
