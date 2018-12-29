#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <timer.h>

void kernel_main(void)
{
    serial_init();
    memory_init();
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
    process_exec("/bin/sh", (char* const*) argv, (char* const*) envp);
}
