#include <abi.h>
#include <pimento.h>

/**
 * This is an over-simplified implementation of the `write` syscall, for debug
 * purposes.
 */
SYSCALL_DEFINE2(write, const char *, src, int, len)
{
    (void) len;

    kputs(src);

    return 0;
}
