#include "abi.h"
#include "asm/entry.h"
#include "asm-generic/unistd.h"
#include "pimento.h"

#define __NR_newfstatat 79
#define __NR_fstat 80

#undef SYSCALL
#define SYSCALL(nr) \
    long abi_ ## nr(long, long, long, long, long, long);

#include "asm/syscalls.h"

#undef SYSCALL
#define SYSCALL(nr) [__NR_ ## nr] = (void *) abi_ ## nr,

static syscall_t _calls[__NR_syscalls] = {
#include "asm/syscalls.h"
};

#undef SYSCALL

/**
 * Handle system calls.
 */
void abi_handler(struct registers * registers, unsigned nr)
{
    if (nr < __NR_syscalls && _calls[nr] != 0) {
        registers->x[0] = _calls[nr](
            registers->x[0],
            registers->x[1],
            registers->x[2],
            registers->x[3],
            registers->x[4],
            registers->x[5]
        );
    } else {
        registers->x[0] = abi_invalid(nr);
    }
}
