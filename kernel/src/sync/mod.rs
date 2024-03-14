pub use self::arc::Arc;
pub use self::arch::*;
pub use self::mutex::Mutex;
pub use self::once::Once;
pub use self::once_lock::OnceLock;

mod arc;
mod mutex;
mod once;
mod once_lock;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
