#pragma once

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>

#include <kstdio.h>
#include <kstdlib.h>
#include <kstring.h>

#include <errno.h>
#include <messages.h>
#include <types.h>

typedef struct {
    uint64_t low;
    uint64_t high;
} uint128_t;
