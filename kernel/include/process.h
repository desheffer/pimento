#pragma once

#define PSR_MODE_INIT    0x3C5
#define PSR_MODE_KTHREAD 0x305
#define PSR_MODE_USER    0x300

#define PNAME_LENGTH 32

#ifndef __ASSEMBLY__

#include <entry.h>
#include <list.h>

enum process_state {
    created,
    running,
    sleeping,
    stopped,
    zombie,
};

struct cpu_context {
    long unsigned regs[11]; // x19 - x29
    long unsigned sp;
    long unsigned pc;
};

struct process {
    unsigned pid;
    enum process_state state;
    char pname[PNAME_LENGTH];
    struct cpu_context * cpu_context;
    struct mm_context * mm_context;
};

typedef void process_function_t(void *);

struct process * process_create_kernel(void);
int process_create(void *, const char *, void *);
void process_create_tail(process_function_t, void *);
void process_create_tail_wrapper(void);
int process_clone(struct process *);
void process_destroy(struct process *);
int process_exec(const char *, char * const[], char * const[]);
void process_exec_tail(const char *, char * const *, char * const *);
void process_exec_tail_wrapper(void);
void * process_set_args(void *, char * const[], char * const[]);

void do_exec(struct registers *);
void process_clone_tail(void);

#endif
