#pragma once

#include <exception.h>
#include <kstdio.h>

#define assert(cond) if (!(cond)) { \
    kprintf("\n\nAssertion failed (%s) at %s:%d.\n", #cond, __FILE__, __LINE__); \
    breakpoint; \
}
