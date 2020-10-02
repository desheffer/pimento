#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <sys/types.h>

#include "kstdio.h"
#include "kstdlib.h"
#include "kstring.h"

#include "errno.h"
#include "messages.h"

typedef struct {
    uint64_t low;
    uint64_t high;
} uint128_t;

#define fail(msg) \
    kputs(FG_RED msg RESET "\n"); \
    while (1);
