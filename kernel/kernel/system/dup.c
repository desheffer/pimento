#include <system.h>

SYSCALL_DEFINE1(dup, int, oldfd)
{
    // @TODO
    return oldfd;
}
