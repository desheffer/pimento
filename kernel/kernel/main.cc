#include <heap.h>
#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <stdint.h>
#include <timer.h>

extern "C" void kernel_main()
{
    Memory::init();

    Heap::init(Memory::instance());

    Serial::init();

    kputs(
        "\n"
        "[44m[97m                          [0m\n"
        "[44m[97m     Welcome to PI-OS     [0m\n"
        "[44m[97m                          [0m\n"
        "\n"
    );

    kprintf("Memory Allocation = %u\n", Memory::instance()->allocSize());
    kprintf("Page Count = %u\n", Memory::instance()->pageCount());

    kprintf("Interrupts: ");
    Interrupt::init();
    kprintf("OK\n");

    kprintf("Process Scheduler: ");
    Scheduler::init();
    kprintf("OK\n");

    kprintf("System Timer: ");
    Timer::init(Interrupt::instance(), Scheduler::instance());
    kprintf("OK\n");

    Scheduler::instance()->spawn();

    printf("\n");
    while (true) {
        Timer::wait(1000);
        printf("#");
    }
}
