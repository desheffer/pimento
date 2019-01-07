#include <system.h>

long sys_brk(void * addr)
{
    (void) addr;

    return -1;
}
