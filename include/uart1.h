#include <stdint.h>

#ifndef UART1_H
#define UART1_H

namespace uart1
{
    void init();
    char getc();
    void putc(char);
    void puts(const char*);
    void putx(uint32_t);
}

#endif
