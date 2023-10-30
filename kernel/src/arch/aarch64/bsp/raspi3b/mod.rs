use crate::io;

pub mod bcm2837_serial;

pub fn init() {
    let serial = bcm2837_serial::serial();
    io::set_console(serial);
}
