#include <bits/ioctl.h>

#include "abi.h"
#include "pimento.h"

SYSCALL_DEFINE3(ioctl, int, fd, long unsigned, request, char *, argp)
{
    // @TODO: Perform the `request`.
    if ((fd == 0 || fd == 1 || fd == 2)
        && (request == TIOCGPGRP || request == TIOCSPGRP || request == TIOCGWINSZ)
    ) {
        (void) argp;

        return 0;
    }

    return -EINVAL;
}
