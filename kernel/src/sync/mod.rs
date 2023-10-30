pub use crate::arch::sync::Lock;

pub use self::mutex::Mutex;
pub use self::once_lock::OnceLock;

mod mutex;
mod once_lock;
