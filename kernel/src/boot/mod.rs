pub use self::arch::*;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
