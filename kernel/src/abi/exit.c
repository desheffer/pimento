#include <abi.h>
#include <exit.h>
#include <pimento.h>

SYSCALL_DEFINE1(exit, int, code)
{
    exit(code);

    return 0;
}
