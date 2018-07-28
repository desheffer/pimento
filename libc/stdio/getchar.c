#include <unistd.h>
#include <stdio.h>

int getchar()
{
    char c;

    read(0, &c, 1);

    return c;
}
