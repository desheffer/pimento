mod bcm2837_serial;

use crate::console;

pub fn init() {
    bcm2837_serial::init();
    console::set_console(&bcm2837_serial::SERIAL_CONSOLE);
}
