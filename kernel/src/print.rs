use core::fmt::{self, Write};

use crate::device::{Logger, Monotonic};
use crate::sync::{Arc, OnceLock};

static LOGGER: OnceLock<Arc<dyn Logger>> = OnceLock::new();
static MONOTONIC: OnceLock<Arc<dyn Monotonic>> = OnceLock::new();

/// Sets the kernel logger.
pub fn set_logger(logger: Arc<dyn Logger>) {
    if LOGGER.set(logger).is_err() {
        panic!("logger already set");
    }
}

/// Sets the monotonic clock to use when logging.
pub fn set_monotonic(monotonic: Arc<dyn Monotonic>) {
    if MONOTONIC.set(monotonic).is_err() {
        panic!("monotonic already set");
    }
}

/// Prints to the kernel logger.
#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {{
        $crate::print::_print(format_args!($($arg)*));
    }};
}

/// Prints to the kernel logger, with a newline.
#[macro_export]
macro_rules! println {
    () => {
        $crate::print::_println(format_args_nl!(""));
    };
    ($($arg:tt)*) => {{
        $crate::print::_println(format_args_nl!($($arg)*));
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
    writer.write_fmt(args).unwrap();
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
        if let Some(logger) = LOGGER.get() {
            logger.write_str(s);
        }
        Ok(())
    }
}
