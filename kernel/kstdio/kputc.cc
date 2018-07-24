#include <serial.h>
#include <stdio.h>
#include <synchronize.h>

void kputs(const char* s)
{
    enter_critical();

    while (*s) {
        Serial::putc(*s);
        ++s;
    }

    leave_critical();
}
