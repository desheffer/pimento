use core::arch::{asm, global_asm};
use core::ptr::addr_of;

use crate::abi::LocalInterruptHandler;
use crate::sync::{Arc, OnceLock};

extern "C" {
    static mut vector_table: u8;
}

/// A vector table manager.
pub struct VectorTable {
    inner: Arc<TableInner>,
}

static INSTANCE: OnceLock<VectorTable> = OnceLock::new();

impl VectorTable {
    /// Gets or initializes the vector table manager.
    pub fn instance() -> &'static Self {
        INSTANCE.get_or_init(|| Self::new(LocalInterruptHandler::instance()))
    }

    fn new(local_interrupt_handler: &'static LocalInterruptHandler) -> Self {
        let inner = Arc::new(TableInner {
            local_interrupt_handler,
        });

        Self { inner }
    }

    /// Installs a vector table pointing to this instance.
    pub unsafe fn install(&self) {
        INSTALLED_TABLE
            .set(self.inner.clone())
            .unwrap_or_else(|_| panic!("installing vector table failed"));

        asm!(
            "msr vbar_el1, {}",
            "isb",
            in(reg) addr_of!(vector_table),
        );
    }
}

/// The inner datum whose memory is managed.
struct TableInner {
    local_interrupt_handler: &'static LocalInterruptHandler,
}

static INSTALLED_TABLE: OnceLock<Arc<TableInner>> = OnceLock::new();

/// Wraps the `handle` function so that it can be called from the vector table.
#[no_mangle]
pub unsafe extern "C" fn _vector_irq() {
    let inner = &**INSTALLED_TABLE.get().unwrap();
    inner.local_interrupt_handler.handle();
}

/// Handles invalid entries from the vector table.
#[no_mangle]
pub extern "C" fn _vector_invalid(code: u64, esr_el1: u64, far_el1: u64) -> ! {
    panic!(
        "invalid vector entry {:#03x} (ESR = {:#018x}, FAR = {:#018x})",
        code, esr_el1, far_el1
    );
}

global_asm!(
    include_str!("vector_table.s"),
    TASK_REGS_SIZE = const 34 * 8,
);
