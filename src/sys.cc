#include <interrupt.h>
#include <stdio.h>
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
    puts("\n\n!!! Kernel Panic! !!!\n\n");
    halt();
}

void __dso_handle()
{
}

void __cxa_atexit(void*, void (*)(void*), void*)
{
}
