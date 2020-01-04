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
        "\e[42m\e[97m                           \e[0m\n"
        "\e[42m\e[97m     All tests passed!     \e[0m\n"
        "\e[42m\e[97m                           \e[0m\n"
        "\n"
    );
}
