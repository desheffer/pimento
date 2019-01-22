#include <system.h>

SYSCALL_DEFINE1(umask, int, mask)
{
    // @TODO
    (void) mask;

    return 0;
}
