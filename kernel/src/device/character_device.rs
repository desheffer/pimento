use alloc::vec::Vec;

/// A character device.
pub trait CharacterDevice {
    /// Reads bytes from the device.
    fn read(&self, count: usize) -> Result<Vec<u8>, ()>;

    /// Writes bytes to the device.
    fn write(&self, buf: &[u8]) -> Result<usize, ()>;
}
