use core::fmt::{self, Write};

use alloc::sync::Arc;

use crate::device::{CharacterDevice, Monotonic};
use crate::sync::OnceLock;

static DEVICE: OnceLock<Arc<dyn CharacterDevice>> = OnceLock::new();
static MONOTONIC: OnceLock<Arc<dyn Monotonic>> = OnceLock::new();

/// Sets the kernel logger.
pub fn set_character_device(logger: Arc<dyn CharacterDevice>) -> Result<(), ()> {
    DEVICE.set(logger).map_err(|_| ())
}

/// Sets the monotonic clock to use when logging.
pub fn set_monotonic(monotonic: Arc<dyn Monotonic>) -> Result<(), ()> {
    MONOTONIC.set(monotonic).map_err(|_| ())
}

/// Prints to the kernel logger.
#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {{
        $crate::log::_print(format_args!($($arg)*));
    }};
}

/// Prints to the kernel logger, with a newline.
#[macro_export]
macro_rules! println {
    () => {
        $crate::log::_println(format_args_nl!(""));
    };
    ($($arg:tt)*) => {{
        $crate::log::_println(format_args_nl!($($arg)*));
    }};
}

/// Prints and returns the value of a given expression for quick debugging.
#[macro_export]
macro_rules! dbg {
    () => {
        $crate::println!("[{}:{}]", file!(), line!())
    };
    ($val:expr $(,)?) => {
        match $val {
            tmp => {
                $crate::println!("[{}:{}] {} = {:#?}", file!(), line!(), stringify!($val), &tmp);
                tmp
            }
        }
    };
    ($($val:expr),+ $(,)?) => {
        ($($crate::dbg!($val)),+,)
    };
}

pub fn _print(args: fmt::Arguments) {
    let mut writer = Writer {};
    let _ = writer.write_fmt(args);
}

pub fn _println(args: fmt::Arguments) {
    if let Some(monotonic) = MONOTONIC.get() {
        let uptime = monotonic.monotonic();
        print!("[{:5}.{:03}] ", uptime.as_secs(), uptime.subsec_millis());
    }

    _print(args);
}

/// A writer for kernel log messages.
struct Writer {}

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        if let Some(logger) = DEVICE.get() {
            let _ = logger.write(s.as_bytes());
        }
        Ok(())
    }
}
