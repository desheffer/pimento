#include <unistd.h>
#include <stdio.h>
#include <string.h>

void puts(const char* s)
{
    write(1, s, strlen(s));
}
