pub use self::arch::*;
pub use self::interrupt_router::InterruptRouter;
pub use self::system_call_router::SystemCallRouter;
pub use self::system_calls::register_system_calls;

mod interrupt_router;
mod system_call_router;
mod system_calls;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
