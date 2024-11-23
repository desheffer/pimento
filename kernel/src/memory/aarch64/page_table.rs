use core::mem::size_of;
use core::ops::{Index, IndexMut};

use crate::memory::PhysicalAddress;

pub type Page = [u8; 4096]; // The size of one page (4 KB)

const ROW_COUNT: usize = size_of::<Page>() / size_of::<TableRow>(); // The number of rows in a translation table

pub const LEVEL_ROOT: i8 = 0; // The root level, where translation starts
pub const LEVEL_MAX: i8 = 3; // The last level, where each row is a page

const TYPE_MASK: u64 = 0b11;
const TYPE_TABLE: u64 = 0b11; // Table
const TYPE_BLOCK: u64 = 0b01; // Block
const TYPE_PAGE: u64 = 0b11; // Page

const TABLE_ADDRESS_MASK: u64 = 0xFFFFFFFFF000; // Output address[47:12]
const BLOCK_ADDRESS_MASK: u64 = 0xFFFFFFE00000; // Output address[47:21]
const PAGE_ADDRESS_MASK: u64 = 0xFFFFFFFFF000; // Output address[47:12]

const UNPRIVILEGED: u64 = 0b1 << 6; // Unprivileged
const READ_ONLY: u64 = 0b1 << 7; // Read-only
const ACCESS_FLAG: u64 = 0b1 << 10; // Access flag (AF)
const NOT_GLOBAL: u64 = 0b1 << 11; // Not global (nG)
const EXECUTE_NEVER: u64 = 0b1 << 54; // Execute-never (XN)

const ATTRIBUTE_SHIFT: u64 = 2;
const ATTRIBUTE_MASK: u64 = 0b111 << ATTRIBUTE_SHIFT;

/// AArch64 memory attributes.
///
/// The numeric index is used to initialize the MAIR_EL1 register.
#[derive(Clone, Copy)]
pub enum Attribute {
    Device = 0,
    Normal = 1,
    NormalNC = 2,
}

/// AArch64 descriptor types.
pub enum DescriptorType {
    Table,
    Block,
    Page,
}

/// An AArch64 translation table row.
type TableRow = u64;

/// An AArch64 translation table.
#[repr(align(4096))] // Align to size of page
#[repr(C)]
pub struct Table {
    rows: [TableRow; ROW_COUNT],
}

impl Table {
    /// Creates a table initialized with all zeroes.
    pub const fn new() -> Self {
        Self {
            rows: [0; ROW_COUNT],
        }
    }
}

impl Index<usize> for Table {
    type Output = TableRow;

    fn index(&self, index: usize) -> &Self::Output {
        assert!(index < ROW_COUNT);
        &self.rows[index]
    }
}

impl IndexMut<usize> for Table {
    fn index_mut(&mut self, index: usize) -> &mut Self::Output {
        assert!(index < ROW_COUNT);
        &mut self.rows[index]
    }
}

/// An AArch64 translation table manager.
pub struct TableManager {
    table: *mut Table,
    level: i8,
    input_address_start: *const (),
}

impl TableManager {
    /// Creates a translation table manager.
    pub fn new(table: *mut Table, level: i8, input_address_start: *const ()) -> Self {
        Self {
            table,
            level,
            input_address_start,
        }
    }

    /// Gets the table.
    pub fn table(&self) -> *mut Table {
        self.table
    }

    /// Gets the level of this table.
    pub fn level(&self) -> i8 {
        self.level
    }

    /// Gets the number of rows in this table.
    pub fn row_count(&self) -> usize {
        ROW_COUNT
    }

    /// Gets the starting address for bytes that are addressed by this table.
    pub fn start(&self) -> *const () {
        self.input_address_start
    }

    /// Gets the number of bytes that are addressed by this table.
    pub fn len(&self) -> usize {
        // The address space for a level 3 table is `size_of::<Page>()` * `ROW_COUNT` bytes. For
        // all other levels, the address space for a level N table is `ROW_COUNT` times larger than
        // a level N+1 table.
        size_of::<Page>() * ROW_COUNT.pow((LEVEL_MAX - self.level + 1) as u32)
    }

    /// Gets the row in this table with the given index.
    pub unsafe fn row(&self, index: usize) -> TableRowManager {
        TableRowManager {
            table_manager: self,
            index,
        }
    }

    /// Gets the row in this table that contains the given address.
    pub unsafe fn row_by_address(&self, address: usize) -> Result<TableRowManager, ()> {
        let start = self.start() as usize;
        let end = start + self.len();
        let row_size = self.len() / ROW_COUNT;

        if address < start || address >= end {
            return Err(());
        }

        Ok(TableRowManager {
            table_manager: self,
            index: (address - start) / row_size,
        })
    }
}

/// An AArch64 translation table row manager.
pub struct TableRowManager<'a> {
    table_manager: &'a TableManager,
    index: usize,
}

impl<'a> TableRowManager<'a> {
    /// Gets the value of this row.
    unsafe fn value(&self) -> TableRow {
        (*self.table_manager.table)[self.index]
    }

    /// Sets the value of this row.
    unsafe fn set_value(&self, value: TableRow) {
        (*self.table_manager.table)[self.index] = value;
    }

    /// Gets the starting address for bytes that are addressed by this row.
    pub fn start(&self) -> *const () {
        (self.table_manager.start() as usize + self.len() * self.index) as _
    }

    /// Gets the number of bytes that are addressed by this row.
    pub fn len(&self) -> usize {
        self.table_manager.len() / ROW_COUNT
    }

    /// Gets the descriptor type of this row or `None` if the row is not initialized.
    pub unsafe fn descriptor_type(&self) -> Option<DescriptorType> {
        let level = self.table_manager.level();

        match self.value() & TYPE_MASK {
            TYPE_TABLE if level < LEVEL_MAX => Some(DescriptorType::Table),
            TYPE_BLOCK if level > 0 => Some(DescriptorType::Block),
            TYPE_PAGE if level == LEVEL_MAX => Some(DescriptorType::Page),
            _ => None,
        }
    }

    /// Writes a table descriptor to this row.
    pub unsafe fn write_table(&self, builder: &TableDescriptorBuilder) -> Result<(), ()> {
        if self.table_manager.level >= LEVEL_MAX {
            return Err(());
        }

        self.set_value(builder.build());
        Ok(())
    }

    /// Writes a block descriptor to this row.
    pub unsafe fn write_block(&self, builder: &BlockDescriptorBuilder) -> Result<(), ()> {
        if self.table_manager.level <= 0 {
            return Err(());
        }

        self.set_value(builder.build());
        Ok(())
    }

    /// Writes a page descriptor to this row.
    pub unsafe fn write_page(&self, builder: &PageDescriptorBuilder) -> Result<(), ()> {
        if self.table_manager.level != LEVEL_MAX {
            return Err(());
        }

        self.set_value(builder.build());
        Ok(())
    }

    /// Loads this row into a table descriptor builder.
    ///
    /// This function will panic if the row does not have a `Page` descriptor.
    pub unsafe fn load_table(&self) -> Result<TableDescriptorBuilder, ()> {
        match self.descriptor_type() {
            Some(DescriptorType::Table) => Ok(TableDescriptorBuilder {
                pending_value: self.value(),
            }),
            _ => Err(()),
        }
    }

    /// Loads this row into a block descriptor builder.
    ///
    /// This function will panic if the row does not have a `Block` descriptor.
    pub unsafe fn load_block(&self) -> Result<BlockDescriptorBuilder, ()> {
        match self.descriptor_type() {
            Some(DescriptorType::Block) => Ok(BlockDescriptorBuilder {
                pending_value: self.value(),
            }),
            _ => Err(()),
        }
    }

    /// Loads this row into a page descriptor builder.
    ///
    /// This function will panic if the row does not have a `Page` descriptor.
    pub unsafe fn load_page(&self) -> Result<PageDescriptorBuilder, ()> {
        match self.descriptor_type() {
            Some(DescriptorType::Page) => Ok(PageDescriptorBuilder {
                pending_value: self.value(),
            }),
            _ => Err(()),
        }
    }
}

/// An AArch64 table descriptor builder.
pub struct TableDescriptorBuilder {
    pending_value: TableRow,
}

impl TableDescriptorBuilder {
    /// Creates a builder with the given address.
    pub fn new(address: PhysicalAddress<Table>) -> Result<Self, ()> {
        if address.address() as u64 & !TABLE_ADDRESS_MASK != 0 {
            return Err(());
        }

        Ok(Self {
            pending_value: TYPE_TABLE | address.address() as u64,
        })
    }

    /// Gets the address contained in this builder.
    pub fn address(&self) -> PhysicalAddress<Table> {
        PhysicalAddress::<Table>::new((self.pending_value & TABLE_ADDRESS_MASK) as _)
    }

    /// Builds a table row.
    fn build(&self) -> TableRow {
        self.pending_value
    }
}

/// An AArch64 block descriptor builder.
pub struct BlockDescriptorBuilder {
    pending_value: TableRow,
}

impl BlockDescriptorBuilder {
    /// Creates a builder with the given address.
    ///
    /// The caller is responsible for ensuring that the size of the block is appropriate.
    pub fn new(address: PhysicalAddress<u8>) -> Result<Self, ()> {
        if address.address() as u64 & !BLOCK_ADDRESS_MASK != 0 {
            return Err(());
        }

        Ok(Self {
            pending_value: TYPE_BLOCK | ACCESS_FLAG | NOT_GLOBAL | address.address() as u64,
        })
    }

    /// Sets the given memory attribute.
    pub fn set_attribute(mut self, attribute: Attribute) -> Self {
        self.pending_value =
            (self.pending_value & !ATTRIBUTE_MASK) | ((attribute as TableRow) << ATTRIBUTE_SHIFT);
        self
    }

    /// Gets the address contained in this builder.
    pub fn address(&self) -> PhysicalAddress<u8> {
        PhysicalAddress::<u8>::new((self.pending_value & BLOCK_ADDRESS_MASK) as _)
    }

    /// Builds a table row.
    fn build(&self) -> TableRow {
        self.pending_value
    }
}

/// An AArch64 page descriptor builder.
pub struct PageDescriptorBuilder {
    pending_value: TableRow,
}

impl PageDescriptorBuilder {
    /// Creates a builder with the given address.
    pub fn new(address: PhysicalAddress<Page>) -> Result<Self, ()> {
        if address.address() as u64 & !PAGE_ADDRESS_MASK != 0 {
            return Err(());
        }

        Ok(Self {
            pending_value: TYPE_PAGE | ACCESS_FLAG | NOT_GLOBAL | address.address() as u64,
        })
    }

    /// Sets the given memory attribute.
    pub fn set_attribute(mut self, attribute: Attribute) -> Self {
        self.pending_value =
            (self.pending_value & !ATTRIBUTE_MASK) | ((attribute as TableRow) << ATTRIBUTE_SHIFT);
        self
    }

    /// Sets the unprivileged value.
    pub fn set_unprivileged(mut self, unprivileged: bool) -> Self {
        self.pending_value = match unprivileged {
            true => self.pending_value | UNPRIVILEGED,
            false => self.pending_value & !UNPRIVILEGED,
        };
        self
    }

    /// Gets the address contained in this builder.
    pub fn address(&self) -> PhysicalAddress<Page> {
        PhysicalAddress::<Page>::new((self.pending_value & PAGE_ADDRESS_MASK) as _)
    }

    /// Builds a table row.
    fn build(&self) -> TableRow {
        self.pending_value
    }
}
