#pragma once

#include <process.h>
#include <stdint.h>

extern "C" void halt();
extern "C" void panic();
extern "C" void debug_process_regs(process_regs_t*, uint64_t, uint64_t);
