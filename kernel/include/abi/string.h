#pragma once

#include <task.h>

size_t strncpy_to_user(struct mm_context *, void *, const char *, size_t);
