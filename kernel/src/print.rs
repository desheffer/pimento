use crate::console;
use core::fmt::{self, Write};

struct Writer;

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        console::console().write_str(s);
        Ok(())
    }
}

pub fn _print(args: fmt::Arguments) {
    let mut writer = Writer {};
    writer.write_fmt(args).unwrap();
}

#[macro_export]
macro_rules! print {
    ($($arg:tt)*) => {{
        $crate::print::_print(format_args!($($arg)*));
    }};
}

#[macro_export]
macro_rules! println {
    () => {
        $crate::print!("\n")
    };
    ($($arg:tt)*) => {{
        $crate::print::_print(format_args_nl!($($arg)*));
    }};
}
