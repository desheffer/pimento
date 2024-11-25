pub use self::lock::Lock;
pub use self::mutex::Mutex;
pub use self::once::Once;
pub use self::once_lock::OnceLock;

mod lock;
mod mutex;
mod once;
mod once_lock;

/// Stores the given expression using a static `OnceLock` and returns a static reference.
#[macro_export]
macro_rules! static_get_or_init {
    ($type:ty, $init:expr $(,)?) => {{
        static ONCE_LOCK: OnceLock<$type> = OnceLock::new();
        ONCE_LOCK.get_or_init(|| $init)
    }};
}
