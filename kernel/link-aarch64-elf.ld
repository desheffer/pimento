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

    __dash_start = .;
    .text.dash : {
        */dash.bin(.data)
    }
    . = ALIGN(4096);
    __dash_end = .;

    __toybox_start = .;
    .text.toybox : {
        */toybox.bin(.data)
    }
    . = ALIGN(4096);
    __toybox_end = .;

    __rodata_start = .;
    .rodata : {
        *(.rodata)
    }
    . = ALIGN(4096);
    __rodata_end = .;

    __data_start = .;
    .data : {
        *(.data)
    }
    . = ALIGN(4096);
    __data_end = .;

    __va_table_start = .;
    .data.va_table : {
        . += 4 * 4096;
    }
    . = ALIGN(4096);
    __va_table_end = .;

    __bss_start = .;
    .bss : {
        *(.bss)
    }
    . = ALIGN(4096);
    __bss_end = .;

    __end = .;

    /DISCARD/ : { *(.comment) *(.gnu*) *(.note*) *(.eh_frame*) }
}
