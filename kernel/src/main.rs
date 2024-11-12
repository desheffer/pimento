#![allow(dead_code)]
#![feature(allocator_api)]
#![feature(coerce_unsized)]
#![feature(dispatch_from_dyn)]
#![feature(format_args_nl)]
#![feature(layout_for_ptr)]
#![feature(unsize)]
#![no_main]
#![no_std]

extern crate alloc;

mod abi;
mod boot;
mod context;
mod cpu;
mod device;
mod fs;
mod kernel;
mod memory;
mod panic;
mod print;
mod sync;
