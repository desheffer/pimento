#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <system.h>
#include <timer.h>

extern "C" void* _binary____shell_build_shell_img_start;

extern "C" void kernel_main()
{
    Serial::init();

    Memory::init();

    System::init();

    Interrupt::init();

    Timer::init(Interrupt::instance());

    Scheduler::init(Timer::instance());

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
}
