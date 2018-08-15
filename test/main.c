#include <kstdio.h>
#include <memory.h>
#include <panic.h>
#include <serial.h>
#include <test.h>

void kernel_main()
{
    serial_init();
    memory_init();

    kprintf("Running tests...\n");
    test_list();
    test_printf();
    test_string();
    kprintf("All tests passed!\n");

    halt();
}
