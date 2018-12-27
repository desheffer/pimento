#pragma once

#include <asm-generic/unistd.h>
#include <process.h>

typedef registers_t* syscall_t(registers_t*);

registers_t* sys_brk(registers_t*);
registers_t* sys_execve(registers_t*);
registers_t* sys_exit(registers_t*);
registers_t* sys_exit_group(registers_t*);
registers_t* sys_ioctl(registers_t*);
registers_t* sys_read(registers_t*);
registers_t* sys_readv(registers_t*);
registers_t* sys_set_tid_address(registers_t*);
registers_t* sys_write(registers_t*);
registers_t* sys_writev(registers_t*);

registers_t* sys_invalid(registers_t*);
registers_t* system_handler(registers_t*, unsigned);
