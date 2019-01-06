#pragma once

struct list_item {
    void * item;
    struct list_item * prev;
    struct list_item * next;
};

struct list {
    struct list_item * front;
    struct list_item * back;
};

void list_clear(struct list *);
unsigned list_count(struct list *);
struct list * list_create(void);
void list_destroy(struct list *);
void * list_pop_back(struct list *);
void * list_pop_front(struct list *);
void list_push_back(struct list *, void *);
void list_push_front(struct list *, void *);
void list_remove(struct list *, void *);
