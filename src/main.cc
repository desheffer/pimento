#include <heap.h>
#include <interrupt.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <stdint.h>
#include <stdio.h>
#include <timer.h>

extern "C" void kernel_main()
{
    Memory::init();

    Heap::init(Memory::instance());

    Serial::init();
    init_printf(0, Serial::putc);

    puts(
        "\n"
        "[44m[97m                          [0m\n"
        "[44m[97m     Welcome to PI-OS     [0m\n"
        "[44m[97m                          [0m\n"
        "\n"
    );

    printf("Memory Allocation = %u\n", Memory::instance()->allocSize());
    printf("Page Count = %u\n", Memory::instance()->pageCount());

    unsigned el;
    asm volatile("mrs %0, currentel" : "=r" (el));
    printf("Execution Level = %u\n", (el >> 2) & 3);

    printf("Interrupts: ");
    Interrupt::init();
    printf("OK\n");

    printf("Process Scheduler: ");
    Scheduler::init();
    printf("OK\n");

    printf("System Timer: ");
    Timer::init(Interrupt::instance(), Scheduler::instance());
    printf("OK\n");

    Scheduler::instance()->spawn();

    printf("\n");
    while (true) {
        printf("#");
        Timer::wait(1000);
    }
}
