#include <stdio.h>

#ifndef ASSERT_H
#define ASSERT_H

#define assert(cond) if (!(cond)) { \
    printf("Assertion failed: (%s), function %s, file %s, line %d.\n", #cond, __FUNCTION__, __FILE__, __LINE__); \
    while (1); \
}

#endif
