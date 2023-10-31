use crate::io;

pub mod bcm2837_serial;

pub fn init() {
    bcm2837_serial::init();
    let serial = bcm2837_serial::serial().unwrap();
    io::set_console(serial);
}
