SECTIONS
{
    . = 0x400000;
    __start = .;

    __text_start = .;
    .text : {
        KEEP(*(.text._start))
        *(.text*)
    }
    . = ALIGN(4096);
    __text_end = .;

    __rodata_start = .;
    .rodata : {
        *(.rodata*)
    }
    . = ALIGN(4096);
    __rodata_end = .;

    __data_start = .;
    .data : {
        *(.data*)
    }
    . = ALIGN(4096);
    __data_end = .;

    __bss_start = .;
    .bss : {
        *(.bss*)
    }
    . = ALIGN(4096);
    __bss_end = .;

    /DISCARD/ : { *(.comment) }

    __end = .;
}
