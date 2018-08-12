#pragma once

#include <process.h>

extern "C" void halt();
extern "C" void panic();
extern "C" void debug_process_regs(process_regs_t*, long unsigned, long unsigned);
