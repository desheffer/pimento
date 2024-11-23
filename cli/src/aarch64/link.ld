__page_size = 4K; # The size of one page

SECTIONS
{
    . = 0x400000;

    .text ALIGN(__page_size) : {
        KEEP(*(.text._start))
        *(.text*)
    }

    .rodata ALIGN(__page_size) : {
        *(.rodata*)
    }

    .data ALIGN(__page_size) : {
        *(.data*)
    }

    .bss ALIGN(__page_size) : {
        *(.bss*)
    }

    /DISCARD/ : { *(.comment) }
}
