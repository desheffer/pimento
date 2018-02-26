#include <stdio.h>
#include <sys.h>

extern "C" void __dso_handle()
{
}

extern "C" void __cxa_atexit(void*, void (*)(void*), void*)
{
}

void halt()
{
    asm volatile("msr daifset, #2");

    while (1) {
        asm volatile("wfi");
    }
}

void panic()
{
    printf("\n\n!!! Kernel Panic! !!!\n");
    halt();
}
