pub use self::write::SysWrite;

mod write;

/// System call errors.
pub enum SystemCallError {
    OperationNotPermitted = 1,       // EPERM
    NoSuchFileOrDirectory = 2,       // ENOENT
    NoSuchProcess = 3,               // ESRCH
    InterruptedSystemCall = 4,       // EINTR
    IOError = 5,                     // EIO
    NoSuchDeviceOrAddress = 6,       // ENXIO
    ArgumentListTooLong = 7,         // E2BIG
    ExecFormatError = 8,             // ENOEXEC
    BadFileNumber = 9,               // EBADF
    NoChildProcesses = 10,           // ECHILD
    TryAgain = 11,                   // EAGAIN
    OutOfMemory = 12,                // ENOMEM
    PermissionDenied = 13,           // EACCES
    BadAddress = 14,                 // EFAULT
    BlockDeviceRequired = 15,        // ENOTBLK
    DeviceOrResourceBusy = 16,       // EBUSY
    FileExists = 17,                 // EEXIST
    CrossDeviceLink = 18,            // EXDEV
    NoSuchDevice = 19,               // ENODEV
    NotADirectory = 20,              // ENOTDIR
    IsADirectory = 21,               // EISDIR
    InvalidArgument = 22,            // EINVAL
    FileTableOverflow = 23,          // ENFILE
    TooManyOpenFiles = 24,           // EMFILE
    NotATypewriter = 25,             // ENOTTY
    TextFileBusy = 26,               // ETXTBSY
    FileTooLarge = 27,               // EFBIG
    NoSpaceLeftOnDevice = 28,        // ENOSPC
    IllegalSeek = 29,                // ESPIPE
    ReadOnlyFileSystem = 30,         // EROFS
    TooManyLinks = 31,               // EMLINK
    BrokenPipe = 32,                 // EPIPE
    MathArgumentOutOfDomain = 33,    // EDOM
    MathResultNotRepresentable = 34, // ERANGE
}
