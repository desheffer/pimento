#pragma once

#include <asm-generic/unistd.h>
#include <process.h>

typedef size_t ssize_t;

typedef process_regs_t* syscall_t(process_regs_t*);

void system_init();
process_regs_t* system_handler(process_regs_t*, unsigned);
void system_register_call(unsigned, syscall_t*);

process_regs_t* do_brk(process_regs_t*);
process_regs_t* do_execve(process_regs_t*);
process_regs_t* do_exit(process_regs_t*);
process_regs_t* do_exit_group(process_regs_t*);
process_regs_t* do_ioctl(process_regs_t*);
process_regs_t* do_read(process_regs_t*);
process_regs_t* do_readv(process_regs_t*);
process_regs_t* do_set_tid_address(process_regs_t*);
process_regs_t* do_write(process_regs_t*);
process_regs_t* do_writev(process_regs_t*);

process_regs_t* do_invalid(process_regs_t*);
