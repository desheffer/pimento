use core::arch::{asm, global_asm};
use core::ptr::NonNull;

use alloc::boxed::Box;

use crate::abi::LocalInterruptHandler;
use crate::sync::{Arc, OnceLock};

extern "C" {
    static mut vector_table: u8;
}

/// A vector table manager.
#[derive(Debug)]
pub struct VectorTable {
    ptr: NonNull<EntryInner>,
}

impl VectorTable {
    pub fn new(local_interrupt_handler: Arc<LocalInterruptHandler>) -> Self {
        let inner = Box::new(EntryInner {
            local_interrupt_handler,
        });

        Self {
            ptr: Box::leak(inner).into(),
        }
    }

    fn inner(&self) -> &EntryInner {
        // SAFETY: Safe because this is a managed pointer.
        unsafe { self.ptr.as_ref() }
    }

    /// Installs a vector table pointing to this instance.
    pub unsafe fn install(&self) {
        let ptr = EntryInnerPtr { ptr: self.ptr };
        INSTALLED_ENTRY.set(ptr).unwrap();

        let vbar_el1 = &vector_table as *const u8;
        asm!(
            "msr vbar_el1, {}",
            "isb",
            in(reg) vbar_el1,
        );
    }
}

/// The inner datum whose memory is managed.
struct EntryInner {
    local_interrupt_handler: Arc<LocalInterruptHandler>,
}

/// Wraps the `handle` function so that it can be called from the vector table.
#[no_mangle]
pub unsafe extern "C" fn _vector_irq() {
    let inner = INSTALLED_ENTRY.get().unwrap().inner();
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

/// A pointer to the inner datum that can be shared between cores.
#[derive(Debug)]
struct EntryInnerPtr {
    ptr: NonNull<EntryInner>,
}

impl EntryInnerPtr {
    fn inner(&self) -> &EntryInner {
        // SAFETY: Safe because this is a managed pointer.
        unsafe { self.ptr.as_ref() }
    }
}

unsafe impl Send for EntryInnerPtr {}

static INSTALLED_ENTRY: OnceLock<EntryInnerPtr> = OnceLock::new();

global_asm!(
    include_str!("vector_table.s"),
    TASK_REGS_SIZE = const 34 * 8,
);
