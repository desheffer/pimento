/// A logger for kernel output.
pub trait Logger: Send + Sync {
    fn write_str(&self, s: &str);
}
