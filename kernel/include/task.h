#pragma once

#include "pimento.h"

enum task_state {
    new,
    running,
    stopped,
};

struct cpu_context;
struct mm_context;
struct vfs_context;

struct task {
    pid_t pid;
    enum task_state state;
    char name[32];
    struct task * parent;
    struct cpu_context * cpu_context;
    struct mm_context * mm_context;
    struct vfs_context * vfs_context;
    int exit_code;
};

struct task * task_create_init(void);
struct task * task_create(unsigned, const char *, struct mm_context *, struct cpu_context *);
