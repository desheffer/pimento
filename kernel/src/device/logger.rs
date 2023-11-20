use core::fmt::Debug;

/// A logger for kernel output.
pub trait Logger: Debug + Send + Sync {
    fn write_str(&self, s: &str);
}
