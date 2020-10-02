#pragma once

#include "binprm.h"
#include "pimento.h"

struct cpu_context;

struct cpu_context * cpu_context_create_init(void);
struct cpu_context * cpu_context_create_binprm(struct binprm *);
struct cpu_context * cpu_context_create_clone(struct cpu_context *, struct mm_context *);
void cpu_context_destroy(struct cpu_context *);
