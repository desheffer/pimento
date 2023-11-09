pub use self::arch::*;
pub use self::scheduler::{Scheduler, Task, TaskId};

mod scheduler;

#[cfg(target_arch = "aarch64")]
#[path = "aarch64/mod.rs"]
mod arch;
