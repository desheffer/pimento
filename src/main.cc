#include <interrupt.h>
#include <scheduler.h>
#include <serial.h>
#include <stdint.h>
#include <stdio.h>
#include <timer.h>

#include <exception.h>

extern "C" void kernel_main()
{
    Serial* serial = Serial::instance();
    serial->init();
    init_printf(0, Serial::putc);

    unsigned el;
    asm volatile("mrs %0, currentel" : "=r" (el));
    printf("Execution Level = %u\n", (el >> 2) & 3);

    printf("Interrupts: ");
    Interrupt* interrupt = Interrupt::instance();
    interrupt->init();
    printf("OK\n");

    printf("Process Scheduler: ");
    Scheduler* scheduler = Scheduler::instance();
    scheduler->init();
    printf("OK\n");

    printf("System Timer: ");
    Timer* timer = Timer::instance();
    timer->init(interrupt, scheduler);
    printf("OK\n");

    scheduler->spawn();

    // breakpoint;

    while (1) {
        printf("\n[proc 1]");
        Timer::wait(1000);
    }
}
