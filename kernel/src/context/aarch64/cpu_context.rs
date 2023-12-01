/// AArch64 registers to persist between context switches.
#[repr(C)]
pub struct CpuContext {
    pub(super) x19: usize,
    pub(super) x20: usize,
    pub(super) x21: usize,
    pub(super) x22: usize,
    pub(super) x23: usize,
    pub(super) x24: usize,
    pub(super) x25: usize,
    pub(super) x26: usize,
    pub(super) x27: usize,
    pub(super) x28: usize,
    pub(super) fp: usize, // x29
    pub(super) lr: usize, // x30
    pub(super) sp: usize,
}

impl CpuContext {
    pub fn zeroed() -> Self {
        Self {
            x19: 0,
            x20: 0,
            x21: 0,
            x22: 0,
            x23: 0,
            x24: 0,
            x25: 0,
            x26: 0,
            x27: 0,
            x28: 0,
            fp: 0,
            lr: 0,
            sp: 0,
        }
    }
}
