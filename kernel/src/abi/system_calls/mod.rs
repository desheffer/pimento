pub use self::write::SysWrite;

mod write;

/// System call errors.
pub enum SystemCallError {
    BadFileNumber = 9,           // EBADF
    BadAddress = 14,             // EFAULT
    FunctionNotImplemented = 38, // ENOSYS
}
