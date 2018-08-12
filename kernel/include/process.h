#pragma once

#define NUM_REGS 31

struct process_regs_t {
    long unsigned x[NUM_REGS];
    long unsigned :64;
    long unsigned spsr;
    long unsigned lr;
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
