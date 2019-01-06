#include <system.h>

void * sys_brk(void * addr)
{
    (void) addr;

    return (void *) -1;
}
