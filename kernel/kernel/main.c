#include <exec.h>
#include <fs.h>
#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <panic.h>
#include <process.h>
#include <scheduler.h>
#include <serial.h>
#include <timer.h>

static void run_init(void)
{
    const char * const argv[] = {"/bin/sh", 0};
    const char * const envp[] = {"HOME=/root", 0};

    process_exec("/bin/sh", (char * const *) argv, (char * const *) envp);
}

void kernel_main(void)
{
    serial_init();
    memory_init();
    interrupt_init();
    timer_init();
    scheduler_init();
    fs_init();

    kputs(
        "\n"
        "[44m[97m                          [0m\n"
        "[44m[97m     Welcome to PI-OS     [0m\n"
        "[44m[97m                          [0m\n"
        "\n\n"
    );

    process_create(run_init, "init", 0);

    while (scheduler_count() > 1) {
        scheduler_context_switch();
    }

    kputs("Halting\n");
    halt();
}
