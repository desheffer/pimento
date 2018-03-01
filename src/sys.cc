#include <stdio.h>
#include <sys.h>

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

void __dso_handle()
{
}

void __cxa_atexit(void*, void (*)(void*), void*)
{
}
