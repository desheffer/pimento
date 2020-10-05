#pragma once

#define list_foreach(list, type, name) \
    for (int (counter_ ## name) = 0; (counter_ ## name) == 0; (counter_ ## name) = 1) \
    for (type (name) = 0; (counter_ ## name) == 0; (counter_ ## name) = 1) \
    for ( \
        struct list_item * (list_item_ ## name) = (list)->front; \
        (list_item_ ## name) != 0 && (((name) = (list_item_ ## name)->item) || 1 == 1); \
        (list_item_ ## name) = (list_item_ ## name)->next \
    )

struct list_item {
    void * item;
    struct list_item * prev;
    struct list_item * next;
};

struct list {
    struct list_item * front;
    struct list_item * back;
    unsigned count;
};

void * list_at(struct list *, unsigned);
void list_clear(struct list *);
unsigned list_count(struct list *);
struct list * list_create(void);
void list_destroy(struct list *);
void * list_peek_back(struct list *);
void * list_peek_front(struct list *);
void * list_pop_back(struct list *);
void * list_pop_front(struct list *);
void list_push_back(struct list *, void *);
void list_push_front(struct list *, void *);
void list_remove(struct list *, void *);
