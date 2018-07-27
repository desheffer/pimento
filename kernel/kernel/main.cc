#include <heap.h>
#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <stdint.h>
#include <timer.h>

extern "C" void* _binary____shell_build_shell_img_start;

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

    kprintf("\n");

    Scheduler::instance()->createProcess("shell", &_binary____shell_build_shell_img_start);

    while (true) {
        Timer::wait(1000);
        kprintf("#");
    }
}
