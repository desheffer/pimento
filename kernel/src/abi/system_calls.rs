use crate::abi::{SystemCall, SystemCallError, SystemCallNumber};

pub struct SysWrite {}

impl SysWrite {
    pub fn new() -> Self {
        Self {}
    }
}

impl SystemCall for SysWrite {
    fn number(&self) -> SystemCallNumber {
        SystemCallNumber::Write
    }

    fn call(
        &self,
        _: usize,
        _: usize,
        _: usize,
        _: usize,
        _: usize,
        _: usize,
    ) -> Result<usize, SystemCallError> {
        crate::print!("<WRITE>");
        Ok(0)
    }
}
