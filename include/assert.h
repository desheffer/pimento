#pragma once

#include <stdio.h>
#include <sys.h>

#define assert(cond) if (!(cond)) { \
    printf("\n\nAssertion failed (%s) at %s:%d.\n", #cond, __FILE__, __LINE__); \
    halt(); \
}
