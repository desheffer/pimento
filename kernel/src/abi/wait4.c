#include <abi.h>
#include <pimento.h>

SYSCALL_DEFINE0(wait4)
{
    return -ENOSYS;
}
