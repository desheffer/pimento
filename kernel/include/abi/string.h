#pragma once

#include "mm_context.h"
#include "pimento.h"

size_t strncpy_to_user(struct mm_context *, void *, const char *, size_t);
