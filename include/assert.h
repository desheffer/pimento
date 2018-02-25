#include <stdio.h>
#include <sys.h>

#ifndef ASSERT_H
#define ASSERT_H

#define assert(cond) if (!(cond)) { \
    printf("\n\nAssertion failed: (%s), function %s, file %s, line %d.\n\n", #cond, __FUNCTION__, __FILE__, __LINE__); \
    halt(); \
}

#endif
