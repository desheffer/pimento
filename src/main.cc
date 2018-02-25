#include <interrupt.h>
#include <scheduler.h>
#include <serial.h>
#include <stdint.h>
#include <stdio.h>
#include <timer.h>

extern "C" void kernel_main()
{
    Serial* serial = Serial::instance();
    Interrupt* interrupt = Interrupt::instance();
    Scheduler* scheduler = Scheduler::instance();
    Timer* timer = Timer::instance();

    serial->init();
    init_printf(0, Serial::putc);

    unsigned el;
    asm volatile("mrs %0, currentel" : "=r" (el));
    printf("Execution Level = %u\n", (el >> 2) & 3);

    printf("Interrupts: ");
    interrupt->init();
    printf("OK\n");

    printf("Process Scheduler: ");
    scheduler->init(timer);
    printf("OK\n");

    printf("System Timer: ");
    timer->init(interrupt);
    printf("OK\n");

    scheduler->spawn();

    while (1) {
        printf("\n[proc 1]");
        Timer::wait(1000);
    }
}
