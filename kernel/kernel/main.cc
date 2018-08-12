#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <panic.h>
#include <scheduler.h>
#include <serial.h>
#include <stdint.h>
#include <timer.h>

extern "C" void* _binary____shell_build_shell_img_start;

extern "C" void kernel_main()
{
    Serial::init();

    Memory::init();

    Interrupt::init();

    Scheduler::init();

    Timer::init(Interrupt::instance(), Scheduler::instance());

    kputs(
        "\n"
        "[44m[97m                          [0m\n"
        "[44m[97m     Welcome to PI-OS     [0m\n"
        "[44m[97m                          [0m\n"
        "\n"
    );

    kprintf("Page Size  = %u\n", (unsigned) Memory::instance()->pageSize());
    kprintf("Page Count = %u\n", Memory::instance()->pageCount());
    kputs("\n");

    Scheduler::instance()->createProcess("shell", &_binary____shell_build_shell_img_start);

    while (Scheduler::instance()->processCount() > 1) {
        asm volatile("wfi");
    }

    kputs("\nWill now halt.\n");
    halt();
}
