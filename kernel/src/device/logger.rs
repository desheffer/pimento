/// A logger for kernel output.
pub trait Logger {
    /// Writes a string to the log.
    fn write_str(&self, s: &str);
}
