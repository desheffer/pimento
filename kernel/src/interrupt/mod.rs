pub use self::arch::*;
pub use self::handler::{Interrupt, LocalInterruptHandler};

mod handler;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
