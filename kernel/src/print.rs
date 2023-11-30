use core::fmt::{self, Write};

use crate::device::{Logger, LoggerImpl, Monotonic, MonotonicImpl};

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {{
        $crate::print::_print(format_args!($($arg)*));
    }};
}

#[macro_export]
macro_rules! println {
    () => {
        $crate::print::_println(format_args_nl!(""));
    };
    ($($arg:tt)*) => {{
        $crate::print::_println(format_args_nl!($($arg)*));
    }};
}

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
    let mut writer = Writer::new(LoggerImpl::instance());
    writer.write_fmt(args).unwrap();
}

pub fn _println(args: fmt::Arguments) {
    let uptime = MonotonicImpl::instance().monotonic();
    print!("[{:5}.{:03}] ", uptime.as_secs(), uptime.subsec_millis());

    _print(args);
}

/// A writer for kernel log messages.
struct Writer<'a> {
    logger: &'a dyn Logger,
}

impl<'a> Writer<'a> {
    fn new(logger: &'a dyn Logger) -> Writer {
        Writer { logger }
    }
}

impl fmt::Write for Writer<'_> {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        self.logger.write_str(s);
        Ok(())
    }
}
