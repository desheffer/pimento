pub mod bsp;
pub mod cpu;
pub mod start;
pub mod sync;

pub fn init() {
    bsp::init();
}
