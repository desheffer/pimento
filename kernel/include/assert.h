#pragma once

#include <kstdio.h>

#define breakpoint asm volatile("brk #0")

#define STRINGIZE_DETAIL(x) #x
#define STRINGIZE(x) STRINGIZE_DETAIL(x)

// Assert against unexpected conditions.
#define assert(cond) \
    if (!(cond)) { \
        kputs("\n\nAssertion failed (" #cond ") at " __FILE__ ":" STRINGIZE(__LINE__) ".\n"); \
        breakpoint; \
    }

// Protect against conditions that are likely to occur, but not implemented.
#define failif(cond) \
    if ((cond)) { \
        kputs("\n\nFailure (" #cond ") at " __FILE__ ":" STRINGIZE(__LINE__) ".\n"); \
        breakpoint; \
    }

#define hexdump(start, length) \
    do_hexdump("Dump at "__FILE__ ":" STRINGIZE(__LINE__) ":\n", start, length)

void do_hexdump(const char *, const void *, size_t);
