#include <serial.h>
#include <stdint.h>
#include <stdio.h>
#include <sys.h>
#include <test.h>

extern "C" void kernel_main()
{
    Serial::instance()->init();
    init_printf(0, Serial::putc);

    printf("Running tests...\n");
    test_list();
    test_printf();
    test_string();
    printf("All tests passed!\n");

    halt();
}
