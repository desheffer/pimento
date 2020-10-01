#pragma once

#include <pimento.h>

struct mm_context;

struct binprm {
    struct mm_context * mm_context;
    void * entry;
    void * stack_bottom;
};
