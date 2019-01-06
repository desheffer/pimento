#include <kstdio.h>
#include <serial.h>
#include <synchronize.h>

void kputs(const char * s)
{
    enter_critical();

    while (*s) {
        serial_putc(*s);
        ++s;
    }

    leave_critical();
}
