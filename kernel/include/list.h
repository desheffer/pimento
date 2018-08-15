#pragma once

typedef struct list_item_t {
    void* item;
    struct list_item_t* prev;
    struct list_item_t* next;
} list_item_t;

typedef struct {
    list_item_t* front;
    list_item_t* back;
} list_t;

void clear(list_t*);
unsigned count(list_t*);
void* pop_back(list_t*);
void* pop_front(list_t*);
void push_back(list_t*, void*);
void push_front(list_t*, void*);
void remove(list_t*, void*);
