#pragma once

#include <pimento.h>

struct fpsimd_context {
    uint128_t q[32];
    uint32_t fpsr;
    uint32_t fpcr;
};

struct cpu_context {
    uint64_t x[11]; // x19 - x29
    uint64_t sp;
    uint64_t pc;
    struct fpsimd_context fpsimd_context;
};

void cpu_switch_to(struct cpu_context *, struct cpu_context *);
void fpsimd_save(struct fpsimd_context *);
void fpsimd_load(struct fpsimd_context *);
void task_entry(void);
