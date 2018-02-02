#include <stdint.h>
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
    uart1::puts("Hello, World!\n");

    while (1) {
        uart1::putc(uart1::getc());
    }
}
