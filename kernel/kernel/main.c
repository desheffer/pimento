#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <process.h>
#include <serial.h>
#include <system.h>
#include <timer.h>

extern char __shell_start;
extern char __shell_end;

// @XXX: Move this block somewhere else...
#include <assert.h>
#include <string.h>
static void init_shell()
{
    memcpy(0x0, &__shell_start, &__shell_end - &__shell_start);

    asm volatile(
        "msr elr_el1, %0\n\t"
        "msr spsr_el1, %1\n\t"
        "mov sp, %2\n\t"
        "eret"
        :
        : "r" (0x0), "r" (PSR_MODE_USER), "r" (STACK_TOP)
    );
}

void kernel_main()
{
    serial_init();
    memory_init();
    system_init();
    interrupt_init();
    timer_init();
    process_init();

    kputs(
        "\n"
        "[44m[97m                          [0m\n"
        "[44m[97m     Welcome to PI-OS     [0m\n"
        "[44m[97m                          [0m\n"
        "\n\n"
    );

    process_create("shell", &init_shell, 0);

    while (process_count() > 1) {
        asm volatile("wfi");
    }

    kputs("\nWill now halt.\n");
}
