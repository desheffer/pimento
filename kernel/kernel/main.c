#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <system.h>
#include <timer.h>
#include <unistd.h>

void two()
{
    while (1) {
        kputs("2");
        asm volatile("wfi");
    }
}

void three()
{
    while (1) {
        kputs("3");
        asm volatile("wfi");
    }
}

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

    process_create("two", two, 0);
    process_create("three", three, 0);

    /* if (fork() == 0) { */
    /*     const char* argv[] = {"/bin/sh", 0}; */
    /*     const char* envp[] = {"PWD=/", 0}; */
    /*     execve("/bin/sh", (char* const*) argv, (char* const*) envp); */
    /* } */

    scheduler_context_switch();

    while (scheduler_count() > 1) {
        asm volatile("wfi");
    }

    kputs("\nWill now halt.\n");
}
