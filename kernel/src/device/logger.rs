/// A logger for kernel output.
pub trait Logger: Send + Sync {
    /// Writes a string to the log.
    fn write_str(&self, s: &str);
}
