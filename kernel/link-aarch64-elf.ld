ENTRY(_start)

SECTIONS
{
    . = 0xFFFF000000080000;
    __start = .;

    __text_start = .;
    .text : {
        KEEP(*(.text.boot))
        *(.text)
    }
    . = ALIGN(4096);
    __text_end = .;

    __rodata_start = .;
    .rodata : {
        *(.rodata)
    }
    . = ALIGN(4096);
    __rodata_end = .;

    __shell_start = .;
    .shell : {
        */shell.bin(.data)
    }
    . = ALIGN(4096);
    __shell_end = .;

    __data_start = .;
    .data : {
        *(.data)
    }
    . = ALIGN(4096);
    __data_end = .;

    __bss_start = .;
    .bss : {
        *(.bss)
    }
    . = ALIGN(4096);
    __bss_end = .;

    __end = .;
    __heap_start = .;

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
