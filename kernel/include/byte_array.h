#pragma once

#include <list.h>
#include <pimento.h>

struct byte_array {
    struct list * pages;
    size_t length;
};

size_t byte_array_append(struct byte_array *, const char *, size_t);
size_t byte_array_copy(struct byte_array *, char *, size_t, size_t);
struct byte_array * byte_array_create(void);
void byte_array_destroy(struct byte_array *);
size_t byte_array_length(struct byte_array *);
