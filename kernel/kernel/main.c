#include <elf.h>
#include <fork.h>
#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <scheduler.h>
#include <serial.h>
#include <system.h>
#include <timer.h>

extern char __shell_start;
extern char __shell_end;

static void init_shell()
{
    void* entry = elf_load(&__shell_start, &__shell_end - &__shell_start);

    move_to_user_mode(entry, (void*) STACK_TOP);
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

    process_create("shell", &init_shell, 0);

    while (scheduler_count() > 1) {
        asm volatile("wfi");
    }

    kputs("\nWill now halt.\n");
}
