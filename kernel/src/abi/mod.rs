pub use self::arch::*;
pub use self::interrupt_handler::{Interrupt, LocalInterruptHandler};

mod interrupt_handler;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
