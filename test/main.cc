#include <stdint.h>
#include <stdio.h>
#include <test.h>
#include <uart1.h>

extern "C" {
    void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags);
}

void kernel_main(uint32_t r0, uint32_t r1, uint32_t atags)
{
    (void) r0;
    (void) r1;
    (void) atags;

    uart1::init();
    init_printf(0, uart1::putc);

    printf("Running tests...\n");
    test_printf();
    test_string();
    printf("All tests passed!\n");

    while (1);
}
