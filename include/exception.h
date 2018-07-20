#pragma once

#include <interrupt.h>
#include <stdint.h>

#define breakpoint asm volatile("brk #0");

extern "C" void exception_handler(process_state_t*, uint64_t, uint64_t, uint64_t);
extern "C" void debug(process_state_t*, uint64_t, uint64_t, uint64_t);
