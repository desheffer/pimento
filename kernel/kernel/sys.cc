#include <interrupt.h>
#include <kstdio.h>
#include <sys.h>

void halt()
{
    disable_interrupts();

    while (true) {
        asm volatile("wfi");
    }
}

void panic()
{
    kputs(
        "\n"
        "[41m[97m                      [0m\n"
        "[41m[97m     Kernel Panic     [0m\n"
        "[41m[97m                      [0m\n"
        "\n"
    );

    halt();
}

void __dso_handle()
{
}

void __cxa_atexit(void*, void (*)(void*), void*)
{
}
