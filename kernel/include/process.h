#pragma once

#include <stdint.h>

#define NUM_REGS 31

struct process_state_t {
    volatile uint64_t x[NUM_REGS];
    volatile uint64_t :64;
    volatile uint64_t spsr;
    volatile uint64_t lr;
};

struct process_control_block_t {
    unsigned pid;
    char pname[32];
    void* stackBegin;
    void* stackEnd;
    process_state_t* state;
};
