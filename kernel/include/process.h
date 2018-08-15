#pragma once

#define NUM_REGS 31

typedef struct {
    long unsigned x[NUM_REGS];
    long unsigned :64;
    long unsigned spsr;
    long unsigned lr;
} process_regs_t;

typedef enum {
    created,
    running,
    sleeping,
    stopping,
} process_state_t;

typedef struct {
    unsigned pid;
    process_state_t state;
    char pname[32];
    void* stack_begin;
    void* stack_end;
    process_regs_t* regs;
} process_control_block_t;
