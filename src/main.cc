#include <stdint.h>
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
    uart1::puts("Hello, World!\n");

    while (1) {
        timer::wait(1000);

        timer::counter_t counter = timer::counter();

        uart1::puts("time: ");
        uart1::putx(counter.high);
        uart1::putx(counter.low);
        uart1::puts("\n");
    }
}
