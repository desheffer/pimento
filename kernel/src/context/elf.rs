use core::cmp::min;
use core::mem::{size_of, transmute_copy};
use core::ops::Range;

use alloc::sync::Arc;
use alloc::vec::Vec;

use crate::fs::File;
use crate::memory::{Page, UserAddress};

const HEADER_IDENT_CLASS_64_BIT: u8 = 0x02;
const HEADER_IDENT_DATA_LE: u8 = 0x01;
const HEADER_IDENT_VERSION_ORIGINAL: u8 = 0x01;
const HEADER_IDENT_ABI_SYSTEM_V: u8 = 0x00;
const HEADER_OBJECT_TYPE_EXECUTABLE: u16 = 0x0002;
const HEADER_MACHINE_AARCH64: u16 = 0x00B7;
const HEADER_VERSION_ORIGINAL: u32 = 0x0000_0001;

const PROGRAM_HEADER_TYPE_LOAD: u32 = 0x0000_0001;

/// An ELF header.
#[repr(C)]
struct Elf64Header {
    ident_magic: [u8; 4],
    ident_class: u8,
    ident_data: u8,
    ident_version: u8,
    ident_abi: u8,
    ident_abi_version: u8,
    ident_pad: [u8; 7],
    object_type: u16,
    machine: u16,
    version: u32,
    entry_point: u64,
    program_header_offset: u64,
    section_header_offset: u64,
    flags: u32,
    header_size: u16,
    program_header_entry_size: u16,
    program_header_entry_count: u16,
    section_header_entry_size: u16,
    section_header_entry_count: u16,
    section_header_string_index: u16,
}

impl Elf64Header {
    /// Returns `true` if the header is valid.
    pub fn is_valid(&self) -> bool {
        self.ident_magic == [0x7F, b'E', b'L', b'F']
            && self.ident_class == HEADER_IDENT_CLASS_64_BIT
            && self.ident_data == HEADER_IDENT_DATA_LE
            && self.ident_version == HEADER_IDENT_VERSION_ORIGINAL
            && self.ident_abi == HEADER_IDENT_ABI_SYSTEM_V
            && self.object_type == HEADER_OBJECT_TYPE_EXECUTABLE
            && self.version == HEADER_VERSION_ORIGINAL
            && self.program_header_entry_size as usize == size_of::<Elf64ProgramHeader>()
    }

    /// Returns `true` if the header has a machine type of AArch64.
    pub fn is_aarch64(&self) -> bool {
        self.machine == HEADER_MACHINE_AARCH64
    }
}

impl From<&[u8; size_of::<Elf64Header>()]> for Elf64Header {
    fn from(value: &[u8; size_of::<Elf64Header>()]) -> Self {
        unsafe { transmute_copy::<[u8; size_of::<Elf64Header>()], Elf64Header>(value) }
    }
}

impl TryFrom<&[u8]> for Elf64Header {
    type Error = ();

    fn try_from(value: &[u8]) -> Result<Self, Self::Error> {
        match value.first_chunk::<{ size_of::<Elf64Header>() }>() {
            Some(chunk) => Ok(chunk.into()),
            None => Err(()),
        }
    }
}

/// An ELF program header.
#[repr(C)]
struct Elf64ProgramHeader {
    segment_type: u32,
    flags: u32,
    file_offset: u64,
    virtual_address: u64,
    physical_address: u64,
    file_size: u64,
    memory_size: u64,
    alignment: u64,
}

impl From<&[u8; size_of::<Elf64ProgramHeader>()]> for Elf64ProgramHeader {
    fn from(value: &[u8; size_of::<Elf64ProgramHeader>()]) -> Self {
        unsafe {
            transmute_copy::<[u8; size_of::<Elf64ProgramHeader>()], Elf64ProgramHeader>(value)
        }
    }
}

impl TryFrom<&[u8]> for Elf64ProgramHeader {
    type Error = ();

    fn try_from(value: &[u8]) -> Result<Self, Self::Error> {
        match value.first_chunk::<{ size_of::<Elf64ProgramHeader>() }>() {
            Some(chunk) => Ok(chunk.into()),
            None => Err(()),
        }
    }
}

/// A reader for ELF files.
pub struct Elf64Reader {
    file: Arc<File>,
    header: Elf64Header,
    program_headers: Vec<Elf64ProgramHeader>,
}

impl Elf64Reader {
    /// Creates an ELF reader.
    pub fn new(file: Arc<File>) -> Result<Self, ()> {
        // Read the ELF header.
        let header: Elf64Header = file.read(size_of::<Elf64Header>())?.as_slice().try_into()?;
        if !header.is_valid() {
            return Err(());
        }

        #[cfg(target_arch = "aarch64")]
        if !header.is_aarch64() {
            return Err(());
        }

        // Read the ELF program headers.
        file.seek(header.program_header_offset as _)?;
        let mut program_headers = Vec::with_capacity(header.program_header_entry_count as _);
        for _ in 0..header.program_header_entry_count {
            let program_header: Elf64ProgramHeader = file
                .read(size_of::<Elf64ProgramHeader>())?
                .as_slice()
                .try_into()?;
            program_headers.push(program_header);
        }

        Ok(Self {
            file,
            header,
            program_headers,
        })
    }

    /// Gets the memory address of the entry point.
    pub fn entry_point(&self) -> usize {
        self.header.entry_point as _
    }

    /// Returns an iterator over page-sized chunks to be loaded into memory.
    pub fn loadable_pages(&self) -> LoadablePages {
        LoadablePages::new(self)
    }
}

/// An iterator over page-sized chunks to be loaded into memory
pub struct LoadablePages<'a> {
    reader: &'a Elf64Reader,
    program_header_index: usize,
    page_index: usize,
}

impl<'a> LoadablePages<'a> {
    /// Creates a loadable page iterator.
    fn new(reader: &'a Elf64Reader) -> Self {
        Self {
            reader,
            program_header_index: 0,
            page_index: 0,
        }
    }
}

impl<'a> LoadablePages<'a> {
    /// Returns a range of file offsets and a range of virtual memory addresses that correspond to
    /// the page with index `page_index` as described by the program header.
    fn range(
        program_header: &Elf64ProgramHeader,
        page_index: usize,
    ) -> Option<(Range<usize>, Range<usize>)> {
        if program_header.segment_type != PROGRAM_HEADER_TYPE_LOAD {
            return None;
        }

        let virtual_address = program_header.virtual_address as usize;
        let file_offset = program_header.file_offset as usize;

        let mut offset_start = page_index * size_of::<Page>();
        let mut offset_end = offset_start + size_of::<Page>();

        // Shift the offsets if the virtual address does not begin on a page boundary.
        let alignment_offset = virtual_address % size_of::<Page>();
        if page_index != 0 {
            offset_start -= alignment_offset;
        }
        offset_end -= alignment_offset;

        // Get the range of this page in the file, bounded by the size of the segment. This range
        // might be empty.
        let file_size = program_header.file_size as usize;
        let file_offset_start = file_offset + min(offset_start, file_size);
        let file_offset_end = file_offset + min(offset_end, file_size);

        // Get the range of this page in memory, bounded by the size of the segment.
        let memory_size = program_header.memory_size as usize;
        let virtual_address_start = virtual_address + min(offset_start, memory_size);
        let virtual_address_end = virtual_address + min(offset_end, memory_size);

        if virtual_address_start == virtual_address_end {
            return None;
        }

        crate::println!("{:#x}..{:#x}", virtual_address_start, virtual_address_end);

        Some((
            file_offset_start..file_offset_end,
            virtual_address_start..virtual_address_end,
        ))
    }
}

impl Iterator for LoadablePages<'_> {
    type Item = LoadablePage;

    fn next(&mut self) -> Option<Self::Item> {
        loop {
            let program_header = self.reader.program_headers.get(self.program_header_index)?;

            // Get the file offset range and virtual address range for the current page.
            let range = match Self::range(program_header, self.page_index) {
                Some(range) => range,
                None => {
                    self.program_header_index += 1;
                    self.page_index = 0;
                    continue;
                }
            };

            // Load the contents of this page using the file offsets.
            self.reader.file.seek(range.0.start).ok()?;
            let mut bytes = self.reader.file.read(range.0.len()).ok()?;

            // If the virtual address range is larger than the file offset range, then pad the
            // vector with zeroes.
            bytes.resize_with(range.1.len(), || 0);

            self.page_index += 1;

            // Return the starting virtual address and the contents of this page.
            let address = UserAddress::new(range.1.start);
            return Some(LoadablePage::new(address, bytes));
        }
    }
}

/// A page-sized chunk to be loaded into memory.
pub struct LoadablePage {
    address: UserAddress<u8>,
    bytes: Vec<u8>,
}

impl LoadablePage {
    /// Creates a loadable page.
    pub fn new(address: UserAddress<u8>, bytes: Vec<u8>) -> Self {
        Self { address, bytes }
    }

    pub fn address(&self) -> UserAddress<u8> {
        self.address
    }

    pub fn bytes(&self) -> &[u8] {
        self.bytes.as_slice()
    }
}
