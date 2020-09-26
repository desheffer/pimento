#pragma once

#include <page.h>
#include <pimento.h>

typedef unsigned pid_t;

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

struct binprm {
    struct mm_context * mm_context;
    void * entry;
    void * stack_bottom;
};

typedef void (* task_function_t)(void *);

struct task * task_create_init(void);
struct task * task_create(const char *, task_function_t, void *);
struct task * task_create_binprm(const char *, struct binprm *, unsigned);

struct cpu_context * cpu_context_create_init(void);
struct cpu_context * cpu_context_create(struct task *, task_function_t, void *);
struct cpu_context * cpu_context_create_user(struct task *, struct binprm *);
void cpu_context_destroy(struct task *);

struct mm_context * mm_context_create_kernel(void);
struct mm_context * mm_context_create_user(void);
void mm_context_destroy(struct mm_context *);
struct page * mm_context_page_alloc(struct mm_context *);
void * mm_context_stack_top(struct mm_context *);
size_t mm_copy_from_user(struct mm_context *, void *, const void *, size_t);
size_t mm_copy_to_user(struct mm_context *, void *, const void *, size_t);

struct vfs_context * vfs_context_create(void);
void vfs_context_destroy(struct vfs_context *);
