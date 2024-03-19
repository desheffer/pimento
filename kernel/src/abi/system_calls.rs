use crate::abi::{SystemCall, SystemCallNumber};
use crate::println;

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

    fn call(&self, _: usize, _: usize, _: usize, _: usize, _: usize, _: usize) -> isize {
        println!("<WRITE>");
        0
    }
}
