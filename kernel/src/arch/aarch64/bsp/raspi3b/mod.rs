use crate::cpu;
use crate::io;
use crate::kernel_main;

pub mod bcm2837_serial;

#[no_mangle]
pub extern "C" fn kernel_init() -> ! {
    bcm2837_serial::init();
    let serial = bcm2837_serial::serial().unwrap();
    io::set_console(serial);

    kernel_main();

    cpu::hang();
}
