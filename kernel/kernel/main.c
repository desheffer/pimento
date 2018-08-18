#include <interrupt.h>
#include <kstdio.h>
#include <memory.h>
#include <process.h>
#include <scheduler.h>
#include <serial.h>
#include <system.h>
#include <timer.h>

extern char _binary____shell_build_shell_img_start;

void kernel_main()
{
    serial_init();
    memory_init();
    system_init();
    interrupt_init();
    timer_init();
    scheduler_init();
    process_init();

    kputs(
        "\n"
        "[44m[97m                          [0m\n"
        "[44m[97m     Welcome to PI-OS     [0m\n"
        "[44m[97m                          [0m\n"
        "\n"
    );

    kprintf("Page Size  = %u\n", (unsigned) memory_page_size());
    kprintf("Page Count = %u\n", memory_page_count());
    kputs("\n");

    process_create("shell", &_binary____shell_build_shell_img_start);

    while (process_count() > 1) {
        asm volatile("wfi");
    }

    kputs("\nWill now halt.\n");
}
