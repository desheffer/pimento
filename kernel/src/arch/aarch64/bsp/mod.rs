pub use self::target_bsp::*;

#[cfg(any(feature = "bsp_raspi3b"))]
#[path = "raspi3b/mod.rs"]
mod target_bsp;
