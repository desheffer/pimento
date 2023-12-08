use core::mem::size_of;

use crate::memory::PhysicalAddress;

pub type Page = [u8; 4 * 1024]; // Pages are 4 KB

const ROW_COUNT: usize = size_of::<Page>() / 8; // Number of rows in a translation table

pub type BlockLevel1 = [u8; size_of::<BlockLevel2>() * ROW_COUNT]; // Block addressed by a level 1 translation table
pub type BlockLevel2 = [u8; size_of::<Page>() * ROW_COUNT]; // Block addressed by a level 2 translation table

const PT_BLOCK: u64 = 0b01; // Block descriptor
const PT_TABLE: u64 = 0b11; // Table descriptor
const PT_PAGE: u64 = 0b11; // Page descriptor
const PT_USER: u64 = 0b1 << 6; // Unprivileged
const PT_RO: u64 = 0b1 << 7; // Read-Only
const PT_OSH: u64 = 0b10 << 8; // Outer Shareable
const PT_ISH: u64 = 0b11 << 8; // Inner Shareable
const PT_AF: u64 = 0b1 << 10; // Access flag
const PT_NG: u64 = 0b1 << 11; // Not global (nG)
const PT_PXN: u64 = 0b1 << 53; // Privileged execute-never
const PT_XN: u64 = 0b1 << 54; // Execute-never

/// AArch64 translation table.
///
/// This is a generic type that covers all translation table levels.
#[repr(align(4096))] // Align to size of page
#[repr(C)]
pub struct Table {
    data: [u64; ROW_COUNT],
}

impl Table {
    pub const fn zeroed() -> Self {
        Self {
            data: [0; ROW_COUNT],
        }
    }

    pub const fn len() -> usize {
        ROW_COUNT
    }

    pub unsafe fn set_row(&mut self, row: usize, builder: &dyn RowBuilder) {
        assert!(row < ROW_COUNT);
        self.data[row] = builder.build();
    }
}

/// A builder for adding rows to a translation table.
pub trait RowBuilder {
    unsafe fn build(&self) -> u64;
}

/// A builder for table rows.
pub struct TableRowBuilder {
    address: usize,
    access_flag: bool,
}

impl TableRowBuilder {
    pub fn new(address: PhysicalAddress<Table>) -> Self {
        Self {
            address: Into::into(address),
            access_flag: false,
        }
    }

    pub fn with_access_flag(mut self, access_flag: bool) -> Self {
        self.access_flag = access_flag;
        self
    }
}

impl RowBuilder for TableRowBuilder {
    unsafe fn build(&self) -> u64 {
        PT_TABLE | self.address as u64 | (if self.access_flag { PT_AF } else { 0 })
    }
}

/// A builder for block rows.
pub struct BlockRowBuilder {
    address: usize,
    attribute: Attribute,
    global: bool,
    access_flag: bool,
}

impl BlockRowBuilder {
    pub fn new<const COUNT: usize>(address: PhysicalAddress<[u8; COUNT]>) -> Self {
        Self {
            address: Into::into(address),
            attribute: Attribute::Device,
            global: false,
            access_flag: false,
        }
    }

    pub fn with_attribute(mut self, attribute: Attribute) -> Self {
        self.attribute = attribute;
        self
    }

    pub fn with_global(mut self, global: bool) -> Self {
        self.global = global;
        self
    }

    pub fn with_access_flag(mut self, access_flag: bool) -> Self {
        self.access_flag = access_flag;
        self
    }
}

impl RowBuilder for BlockRowBuilder {
    unsafe fn build(&self) -> u64 {
        PT_BLOCK
            | self.address as u64
            | (if self.global { 0 } else { PT_NG })
            | (if self.access_flag { PT_AF } else { 0 })
            | self.attribute as u64
    }
}

/// A builder for page rows.
pub struct PageRowBuilder {
    address: usize,
    attribute: Attribute,
    global: bool,
    access_flag: bool,
}

impl PageRowBuilder {
    pub fn new(address: PhysicalAddress<Page>) -> Self {
        Self {
            address: Into::into(address),
            attribute: Attribute::Device,
            global: false,
            access_flag: false,
        }
    }

    pub fn with_attribute(mut self, attribute: Attribute) -> Self {
        self.attribute = attribute;
        self
    }

    pub fn with_global(mut self, global: bool) -> Self {
        self.global = global;
        self
    }

    pub fn with_access_flag(mut self, access_flag: bool) -> Self {
        self.access_flag = access_flag;
        self
    }
}

impl RowBuilder for PageRowBuilder {
    unsafe fn build(&self) -> u64 {
        PT_PAGE
            | self.address as u64
            | (if self.global { 0 } else { PT_NG })
            | (if self.access_flag { PT_AF } else { 0 })
            | self.attribute as u64
    }
}

/// AArch64 memory attributes.
///
/// The numeric index is used when initializing the MAIR_EL1 register.
#[derive(Clone, Copy, Eq, Ord, PartialEq, PartialOrd)]
pub enum Attribute {
    Device = 0, // nGnRnE (no gathering, no re-ordering, and no early write)
    Normal = 1,
    NormalNC = 2,
}
