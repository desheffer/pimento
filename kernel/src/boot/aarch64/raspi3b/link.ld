__page_size = 4K; # The size of one page
__page_row_count = 512; # The number of rows in a translation table
__l3_table_count = 3; # The number of level 3 translation tables configured in `virtual_memory_early_init`

MEMORY
{
    RAM (rwx) : ORIGIN = 0xFFFF000000000000, LENGTH = __page_size * __page_row_count * __l3_table_count
}

SECTIONS
{
    .stack (NOLOAD): {
        . += 0x80000;
        __stack_start = .;
    }

    .text ALIGN(__page_size) : {
        KEEP(*(.text._start))
        KEEP(*(.text._entry))
        *(.text*)
    }

    .rodata ALIGN(__page_size) : {
        *(.rodata*)
    }

    .data ALIGN(__page_size) : {
        *(.data*)
    }

    .bss ALIGN(__page_size) : {
        __bss_start = .;
        *(.bss*)
        __bss_end = .;
    }

    __end = ORIGIN(RAM) + LENGTH(RAM);

    /DISCARD/ : { *(.comment) }
}
