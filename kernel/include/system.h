#pragma once

#include <asm-generic/unistd.h>
#include <process.h>

typedef size_t ssize_t;

typedef void syscall_t(process_regs_t*);

void system_init();
void system_handler(process_regs_t*, unsigned);
void system_register_call(unsigned, syscall_t*);

void do_exit(process_regs_t*);
void do_exit_group(process_regs_t*);
void do_ioctl(process_regs_t*);
void do_read(process_regs_t*);
void do_readv(process_regs_t*);
void do_set_tid_address(process_regs_t*);
void do_write(process_regs_t*);
void do_writev(process_regs_t*);

void do_invalid(process_regs_t*);
