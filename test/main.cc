#include <heap.h>
#include <kstdio.h>
#include <memory.h>
#include <serial.h>
#include <stdint.h>
#include <sys.h>
#include <test.h>

extern "C" void kernel_main()
{
    Memory::init();

    Heap::init(Memory::instance());

    Serial::init();

    kprintf("Running tests...\n");
    test_list();
    test_printf();
    test_string();
    kprintf("All tests passed!\n");

    halt();
}
