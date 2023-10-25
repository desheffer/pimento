use crate::console;

mod bcm2837_serial;

pub fn init() {
    bcm2837_serial::init();
    console::set_console(&bcm2837_serial::SERIAL_CONSOLE);
}
