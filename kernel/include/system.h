#pragma once

#include <process.h>
#include <unistd.h>

#define SYSCALL_COUNT 292

typedef void syscall_t(process_regs_t*);

void system_init();
void system_handler(unsigned, process_regs_t*);
void system_register_call(unsigned, syscall_t*);

void do_exit(process_regs_t*);
void do_read(process_regs_t*);
void do_write(process_regs_t*);

void do_invalid(process_regs_t*);
