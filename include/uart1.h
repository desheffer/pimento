#include <stdint.h>

#ifndef UART1_H
#define UART1_H

namespace uart1
{
    void init();
    char getc();
    void putc(void*, char);
}

#endif
