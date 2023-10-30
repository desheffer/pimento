pub use self::target_arch::*;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod target_arch;
