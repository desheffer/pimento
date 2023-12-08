pub use self::arch::*;
pub use self::page::{PageAllocation, PageAllocator};

mod heap;
mod page;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
