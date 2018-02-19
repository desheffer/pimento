#include <irq.h>
#include <main.h>
#include <stdint.h>
#include <stdio.h>
#include <timer.h>
#include <uart1.h>

void kernel_main()
{
    uart1::init();
    init_printf(0, uart1::putc);

    uint32_t el;
    asm volatile("mrs %0, currentel" : "=r" (el));
    printf("Execution Level = %u\n", (el >> 2) & 3);

    printf("IRQs: ");
    irq::init();
    printf("OK\n");

    printf("System Timer: ");
    timer::init();
    printf("OK\n");

    while (1) {
        printf(".");
        timer::wait(1000);
    }
}
