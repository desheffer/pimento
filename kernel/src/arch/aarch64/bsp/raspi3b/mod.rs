use crate::console;

mod bcm2837_serial;

pub fn init() {
    let serial = bcm2837_serial::serial();
    console::set_console(serial);
}
