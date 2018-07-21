#pragma once

#include <kstdio.h>

#define breakpoint asm volatile("brk #0");

#define assert(cond) if (!(cond)) { \
    kprintf("\n\nAssertion failed (%s) at %s:%d.\n", #cond, __FILE__, __LINE__); \
    breakpoint; \
}
