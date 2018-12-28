#include <kstdio.h>
#include <memory.h>
#include <serial.h>
#include <test.h>

void kernel_main(void)
{
    serial_init();
    memory_init();

    kputs("Running tests...\n");

    test_list();
    test_printf();
    test_string();

    kputs(
        "\n"
        "[42m[97m                           [0m\n"
        "[42m[97m     All tests passed!     [0m\n"
        "[42m[97m                           [0m\n"
        "\n"
    );
}
