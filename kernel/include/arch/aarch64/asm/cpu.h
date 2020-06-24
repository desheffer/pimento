#pragma once

#include <pimento.h>

#define STACK_TOP_USER 0x100000000

struct cpu_context {
    uint64_t x[11]; // x19 - x29
    uint64_t sp;
    uint64_t pc;
};

void cpu_switch_to(struct cpu_context *, struct cpu_context *);
void task_entry(void);
