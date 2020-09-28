#include <abi.h>
#include <pimento.h>

SYSCALL_DEFINE2(mmap, void *, addr, size_t, length)
{
    (void) addr;
    (void) length;

    return -ENOSYS;
}
