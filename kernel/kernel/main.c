#include <elf.h>
#include <fork.h>
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

    /* if (fork() == 0) { */
        char* const argv[] = {0};
        char* const envp[] = {0};
        execve("/bin/sh", argv, envp);
    /* } */

    /* while (scheduler_count() > 1) { */
    /*     asm volatile("wfi"); */
    /* } */

    kputs("\nWill now halt.\n");
}
