#include <system.h>

SYSCALL_DEFINE1(set_tid_address, int *, tidptr)
{
    // @TODO
    (void) tidptr;

    return 0;
}
