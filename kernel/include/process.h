#pragma once

#define PSR_MODE_INIT    0x3C5
#define PSR_MODE_KTHREAD 0x305
#define PSR_MODE_USER    0x300

#define PNAME_LENGTH 32

#define STACK_TOP 0x40000000

#ifndef __ASSEMBLY__

#include <entry.h>
#include <list.h>

typedef enum {
    created,
    running,
    sleeping,
    zombie,
} process_state_t;

typedef struct cpu_context_t {
    long unsigned regs[11]; // x19 - x29
    long unsigned sp;
    long unsigned pc;
} cpu_context_t;

typedef struct mm_context_t mm_context_t;

typedef struct process_t {
    short unsigned pid;
    process_state_t state;
    char pname[PNAME_LENGTH];
    cpu_context_t* cpu_context;
    mm_context_t* mm_context;
} process_t;

process_t* process_create_kernel(void);
void process_destroy(process_t*);
int process_clone(process_t*);
int process_exec(const char*, char* const[], char* const[]);
void* process_set_args(void*, char* const[], char* const[]);

void do_exec(registers_t*);
void fork_tail();

#endif
