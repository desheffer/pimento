#include <unistd.h>
#include <stdio.h>

void putc(const char c)
{
    write(1, &c, 1);
}
