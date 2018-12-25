#pragma once

#include <asm-generic/unistd.h>
#include <process.h>

typedef size_t ssize_t;

typedef registers_t* syscall_t(registers_t*);

void system_init();
registers_t* system_handler(registers_t*, unsigned);
void system_register_call(unsigned, syscall_t*);

registers_t* do_brk(registers_t*);
registers_t* do_execve(registers_t*);
registers_t* do_exit(registers_t*);
registers_t* do_exit_group(registers_t*);
registers_t* do_ioctl(registers_t*);
registers_t* do_read(registers_t*);
registers_t* do_readv(registers_t*);
registers_t* do_set_tid_address(registers_t*);
registers_t* do_write(registers_t*);
registers_t* do_writev(registers_t*);

registers_t* do_invalid(registers_t*);
