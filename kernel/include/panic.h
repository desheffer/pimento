#pragma once

#include <process.h>

void debug_process_regs(registers_t*, long unsigned, long unsigned, long unsigned);
void halt();
void panic();
