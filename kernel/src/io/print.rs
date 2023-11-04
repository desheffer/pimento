use core::fmt::{self, Write};

use crate::io::console;

#[derive(Debug)]
struct Writer;

impl fmt::Write for Writer {
    fn write_str(&mut self, s: &str) -> fmt::Result {
        if let Some(console) = console::console() {
            console.write_str(s);
        }
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
        $crate::io::_print(format_args!($($arg)*));
    }};
}

#[macro_export]
macro_rules! println {
    () => {
        $crate::print!("\n")
    };
    ($($arg:tt)*) => {{
        let uptime = crate::time::Counter::instance().uptime();
        $crate::print!("[{:5}.{:03}] ", uptime.as_secs(), uptime.subsec_millis());
        $crate::io::_print(format_args_nl!($($arg)*));
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
