#pragma once

typedef unsigned pid_t;

enum task_state {
    new,
    running,
    stopped,
};

struct cpu_context;

struct task {
    pid_t pid;
    enum task_state state;
    char name[32];
    struct task * parent;
    struct cpu_context * cpu_context;
};

typedef void task_function_t(void *);

struct task * task_create_init(void);
struct task * task_create(const char *, task_function_t, void *);

void cpu_context_create_init(struct task *);
void cpu_context_create(struct task *, task_function_t, void *);
void cpu_context_destroy(struct task *);

void mm_context_create_init(struct task *);
void mm_context_create(struct task *);
