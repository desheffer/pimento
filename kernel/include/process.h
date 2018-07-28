#pragma once

#include <stdint.h>

#define NUM_REGS 31

struct process_regs_t {
    volatile uint64_t x[NUM_REGS];
    volatile uint64_t :64;
    volatile uint64_t spsr;
    volatile uint64_t lr;
};

enum process_state_t {
    created,
    running,
    sleeping,
    stopping,
};

struct process_control_block_t {
    unsigned pid;
    process_state_t state;
    char pname[32];
    void* stackBegin;
    void* stackEnd;
    process_regs_t* regs;
};
