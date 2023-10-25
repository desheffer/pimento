use crate::mutex::Mutex;

pub trait Console {
    fn write_str(&self, s: &str);

    fn read_byte(&self) -> Option<u8>;
}

struct NullConsole;

impl Console for NullConsole {
    fn write_str(&self, _s: &str) {}

    fn read_byte(&self) -> Option<u8> {
        None
    }
}

static NULL_CONSOLE: NullConsole = NullConsole {};
static CONSOLE: Mutex<&dyn Console> = Mutex::new(&NULL_CONSOLE);

pub fn set_console(console: &'static dyn Console) {
    *CONSOLE.lock() = console;
}

pub fn console() -> &'static dyn Console {
    *CONSOLE.lock()
}