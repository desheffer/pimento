pub use self::arch::*;
pub use self::interrupt_handler::LocalInterruptHandler;

mod interrupt_handler;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
