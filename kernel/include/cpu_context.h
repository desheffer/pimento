#pragma once

#include "binprm.h"
#include "pimento.h"

struct cpu_context;

struct cpu_context * cpu_context_create_init(void);
struct cpu_context * cpu_context_create_binprm(struct binprm *);
void cpu_context_destroy(struct cpu_context *);
