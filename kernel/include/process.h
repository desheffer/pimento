#pragma once

#define PSR_MODE_INIT    0x3C5
#define PSR_MODE_KTHREAD 0x305
#define PSR_MODE_USER    0x300

#define PNAME_LENGTH 32

#ifndef __ASSEMBLY__

#include <entry.h>
#include <list.h>
#include <stdbool.h>

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
    struct fs_context * fs_context;
};

typedef void process_function_t(void *);

struct process * process_create_common(const char *, int, bool);
struct process * process_create_kernel(void);
int process_create(void *, const char *, void *);
int process_clone(struct process *);
void process_destroy(struct process *);

void do_exec(struct registers *);
void process_tail_wrapper(void);

#endif
