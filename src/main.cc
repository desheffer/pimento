#include <stdint.h>
#include <stdio.h>
#include <timer.h>
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

    printf("Hello, World!\n");

    while (1) {
        timer::wait(1000);

        timer::counter_t counter = timer::counter();
        printf("time: %08x %08x\n", counter.high, counter.low);
    }
}
