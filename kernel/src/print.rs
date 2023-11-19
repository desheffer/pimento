use core::fmt::{self, Write};

use crate::device::{Counter, Logger};
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
    if let Some(counter) = PrintRegistry::counter() {
        let uptime = counter.uptime();
        print!("[{:5}.{:03}] ", uptime.as_secs(), uptime.subsec_millis());
    }

    _print(args);
}

/// A registry supporting different print implementations
#[derive(Debug)]
pub struct PrintRegistry {
    logger: OnceLock<Arc<dyn Logger>>,
    counter: OnceLock<Arc<dyn Counter>>,
}

impl PrintRegistry {
    fn instance() -> &'static Self {
        static INSTANCE: PrintRegistry = PrintRegistry::new();
        &INSTANCE
    }

    const fn new() -> Self {
        Self {
            logger: OnceLock::new(),
            counter: OnceLock::new(),
        }
    }

    pub fn set_counter(counter: Arc<dyn Counter>) {
        Self::instance().counter.set(counter).unwrap();
    }

    pub fn set_logger(logger: Arc<dyn Logger>) {
        Self::instance().logger.set(logger).unwrap();
    }

    pub fn counter() -> Option<&'static dyn Counter> {
        Self::instance().counter.get().map(|v| &**v)
    }

    pub fn logger() -> Option<&'static dyn Logger> {
        Self::instance().logger.get().map(|v| &**v)
    }

    pub fn writer() -> Writer {
        Writer::new(Self::instance())
    }
}

/// A writer for kernel log messages
#[derive(Debug)]
pub struct Writer {
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
