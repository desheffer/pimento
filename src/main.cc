#include <interrupt.h>
#include <serial.h>
#include <stdint.h>
#include <stdio.h>
#include <timer.h>

extern "C" void kernel_main()
{
    Serial* serial = Serial::instance();
    serial->init();
    init_printf(0, Serial::putc);

    uint32_t el;
    asm volatile("mrs %0, currentel" : "=r" (el));
    printf("Execution Level = %u\n", (el >> 2) & 3);

    printf("Interrupts: ");
    Interrupt* interrupt = Interrupt::instance();
    interrupt->init();
    printf("OK\n");

    printf("System Timer: ");
    Timer* timer = Timer::instance();
    timer->init(interrupt);
    printf("OK\n");

    while (1) {
        printf(".");
        Timer::wait(1000);
    }
}
