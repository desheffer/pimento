#include <abi.h>
#include <pimento.h>

SYSCALL_DEFINE0(umask)
{
    return -ENOSYS;
}
