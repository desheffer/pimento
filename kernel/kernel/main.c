#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <system.h>
#include <timer.h>
#include <unistd.h>

void kernel_main()
{
    serial_init();
    memory_init();
    system_init();
    interrupt_init();
    timer_init();
    scheduler_init();

    kputs(
        "\n"
        "[44m[97m                          [0m\n"
        "[44m[97m     Welcome to PI-OS     [0m\n"
        "[44m[97m                          [0m\n"
        "\n\n"
    );

    scheduler_context_switch();

    const char* argv[] = {"/bin/sh", 0};
    const char* envp[] = {"PWD=/", 0};
    execve("/bin/sh", (char* const*) argv, (char* const*) envp);

    kputs("\nWill now halt.\n");
}
