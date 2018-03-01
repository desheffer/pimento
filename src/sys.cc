#include <interrupt.h>
#include <stdio.h>
#include <sys.h>

void halt()
{
    disable_interrupts();

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
