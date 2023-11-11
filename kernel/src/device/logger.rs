/// A logger for kernel output
pub trait Logger: Sync {
    fn write_str(&self, s: &str);
}
