use core::mem::size_of;

use crate::memory::PhysicalAddress;

pub type Page = [u8; 4 * 1024]; // Pages are 4 KB

const ROW_COUNT: usize = size_of::<Page>() / 8; // Number of rows in a translation table

const DESC_TABLE: u64 = 0b11; // Table descriptor (lookup levels < 3)
const DESC_BLOCK: u64 = 0b01; // Block descriptor
const DESC_PAGE: u64 = 0b11; // Page descriptor (lookup level 3)

const UNPRIVILEGED: u64 = 0b1 << 6; // Unprivileged
const READ_ONLY: u64 = 0b1 << 7; // Read-only
const OUTER_SHAREABLE: u64 = 0b10 << 8; // Outer Shareable
const INNER_SHAREABLE: u64 = 0b11 << 8; // Inner Shareable
const ACCESS_FLAG: u64 = 0b1 << 10; // Access flag (AF)
const NOT_GLOBAL: u64 = 0b1 << 11; // Not global (nG)
const EXECUTE_NEVER: u64 = 0b1 << 54; // Execute-never (XN)

pub type BlockLevel0 = [u8; size_of::<BlockLevel1>() * ROW_COUNT]; // Block addressed by a level 0 translation table
pub type BlockLevel1 = [u8; size_of::<BlockLevel2>() * ROW_COUNT]; // Block addressed by a level 1 translation table
pub type BlockLevel2 = [u8; size_of::<BlockLevel3>() * ROW_COUNT]; // Block addressed by a level 2 translation table
pub type BlockLevel3 = Page; // Block addressed by a level 3 translation table

/// An AArch64 translation table.
///
/// This is a generic type that covers all translation table levels.
#[repr(align(4096))] // Align to size of page
#[repr(C)]
pub struct Table {
    data: [u64; ROW_COUNT],
}

impl Table {
    /// Creates a table initialized with all zeroes.
    pub const fn zeroed() -> Self {
        Self {
            data: [0; ROW_COUNT],
        }
    }

    /// Returns the length (row count) of the table.
    pub const fn len() -> usize {
        ROW_COUNT
    }

    /// Sets the value of a row using a row builder.
    pub unsafe fn set_row(&mut self, row: usize, builder: &dyn RowBuilder) {
        assert!(row < ROW_COUNT);
        self.data[row] = builder.build();
    }
}

/// A builder for adding rows to a translation table.
pub trait RowBuilder {
    /// Builds the row value.
    unsafe fn build(&self) -> u64;
}

/// A builder for table rows.
pub struct TableRowBuilder {
    address: usize,
}

impl TableRowBuilder {
    /// Creates a table row builder.
    pub fn new(address: PhysicalAddress<Table>) -> Self {
        Self {
            address: Into::into(address),
        }
    }
}

impl RowBuilder for TableRowBuilder {
    unsafe fn build(&self) -> u64 {
        DESC_TABLE | self.address as u64
    }
}

/// A builder for block rows.
pub struct BlockRowBuilder {
    address: usize,
    attribute: Attribute,
    global: bool,
    unprivileged: bool,
}

impl BlockRowBuilder {
    /// Creates a block row builder.
    pub fn new<const COUNT: usize>(address: PhysicalAddress<[u8; COUNT]>) -> Self {
        Self {
            address: Into::into(address),
            attribute: Attribute::Normal,
            global: false,
            unprivileged: false,
        }
    }

    /// Sets the given memory attribute.
    pub fn with_attribute(mut self, attribute: Attribute) -> Self {
        self.attribute = attribute;
        self
    }

    /// Sets the given global value.
    pub fn with_global(mut self, global: bool) -> Self {
        self.global = global;
        self
    }

    /// Sets the given unprivileged value.
    pub fn with_unprivileged(mut self, unprivileged: bool) -> Self {
        self.unprivileged = unprivileged;
        self
    }
}

impl RowBuilder for BlockRowBuilder {
    unsafe fn build(&self) -> u64 {
        DESC_BLOCK
            | self.address as u64
            | (if self.global { 0 } else { NOT_GLOBAL })
            | (if self.unprivileged { UNPRIVILEGED } else { 0 })
            | self.attribute as u64
            | ACCESS_FLAG
    }
}

/// A builder for page rows.
pub struct PageRowBuilder {
    address: usize,
    attribute: Attribute,
    global: bool,
    unprivileged: bool,
}

impl PageRowBuilder {
    /// Creates a page row builder.
    pub fn new(address: PhysicalAddress<Page>) -> Self {
        Self {
            address: Into::into(address),
            attribute: Attribute::Normal,
            global: false,
            unprivileged: false,
        }
    }

    /// Sets the given memory attribute.
    pub fn with_attribute(mut self, attribute: Attribute) -> Self {
        self.attribute = attribute;
        self
    }

    /// Sets the given global value.
    pub fn with_global(mut self, global: bool) -> Self {
        self.global = global;
        self
    }

    /// Sets the given unprivileged value.
    pub fn with_unprivileged(mut self, unprivileged: bool) -> Self {
        self.unprivileged = unprivileged;
        self
    }
}

impl RowBuilder for PageRowBuilder {
    unsafe fn build(&self) -> u64 {
        DESC_PAGE
            | self.address as u64
            | (if self.global { 0 } else { NOT_GLOBAL })
            | (if self.unprivileged { UNPRIVILEGED } else { 0 })
            | self.attribute as u64
            | ACCESS_FLAG
    }
}

/// AArch64 memory attributes.
///
/// The numeric index is used when initializing the MAIR_EL1 register.
#[derive(Clone, Copy)]
pub enum Attribute {
    Device = 0,
    Normal = 1,
    NormalNC = 2,
}
