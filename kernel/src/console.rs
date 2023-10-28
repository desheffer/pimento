use crate::mutex::Mutex;

pub trait Console: Sync {
    fn write_str(&self, s: &str);

    fn read_byte(&self) -> Option<u8>;
}

static CONSOLE: Mutex<Option<&dyn Console>> = Mutex::new(None);

pub fn set_console(console: &'static dyn Console) {
    *CONSOLE.lock() = Some(console);
}

pub fn console() -> Option<&'static dyn Console> {
    *CONSOLE.lock()
}
