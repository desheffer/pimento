use core::fmt::{self, Write};

use crate::device::{Logger, Monotonic};
use crate::sync::{Arc, OnceLock};

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
    PrintRegistry::writer().write_fmt(args).unwrap();
}

pub fn _println(args: fmt::Arguments) {
    if let Some(monotonic) = PrintRegistry::monotonic() {
        let uptime = monotonic.monotonic();
        print!("[{:5}.{:03}] ", uptime.as_secs(), uptime.subsec_millis());
    }

    _print(args);
}

/// A registry supporting different print implementations.
pub struct PrintRegistry {
    logger: OnceLock<Arc<dyn Logger>>,
    monotonic: OnceLock<Arc<dyn Monotonic>>,
}

impl PrintRegistry {
    fn instance() -> &'static Self {
        static INSTANCE: PrintRegistry = PrintRegistry::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            logger: OnceLock::new(),
            monotonic: OnceLock::new(),
        }
    }

    pub fn set_monotonic(monotonic: Arc<dyn Monotonic>) {
        Self::instance().monotonic.set(monotonic).unwrap_or_else(|_| panic!("setting monotonic failed"))
    }

    pub fn set_logger(logger: Arc<dyn Logger>) {
        Self::instance().logger.set(logger).unwrap_or_else(|_| panic!("setting monotonic failed"))
    }

    pub fn monotonic() -> Option<&'static dyn Monotonic> {
        Self::instance().monotonic.get().map(|v| &**v)
    }

    pub fn logger() -> Option<&'static dyn Logger> {
        Self::instance().logger.get().map(|v| &**v)
    }

    fn writer() -> Writer {
        Writer::new(Self::instance())
    }
}

/// A writer for kernel log messages.
struct Writer {
    print_registry: &'static PrintRegistry,
}

impl Writer {
    fn new(print_registry: &'static PrintRegistry) -> Writer {
        Writer { print_registry }
    }
}

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        if let Some(logger) = PrintRegistry::logger() {
            logger.write_str(s);
        }
        Ok(())
    }
}
