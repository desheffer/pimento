SECTIONS
{
    LD_TEXT_START = .;
    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096);
    LD_TEXT_END = .;

    LD_RODATA_START = .;
    .rodata : {
        *(.rodata)
    }
    . = ALIGN(4096);
    LD_RODATA_END = .;

    LD_DATA_START = .;
    .data : {
        *(.data)
    }
    . = ALIGN(4096);
    LD_DATA_END = .;

    LD_BSS_START = .;
    .bss : {
        *(.bss)
    }
    . = ALIGN(4096);
    LD_BSS_END = .;

    LD_PAGE_START = .;

    /DISCARD/ : { *(.comment) *(.note*) *(.eh_frame*) }
}
