#pragma once

#define AUXV_COUNT 1

#define AT_NULL   0
#define AT_PAGESZ 6

#include "task.h"

struct task * exec(struct task *, const char *, char * const *, char * const *);
