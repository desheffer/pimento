#pragma once

typedef struct list_item_t {
    void* item;
    struct list_item_t* prev;
    struct list_item_t* next;
} list_item_t;

typedef struct list_t {
    list_item_t* front;
    list_item_t* back;
} list_t;

void list_clear(list_t*);
unsigned list_count(list_t*);
void list_delete(list_t*);
list_t* list_new();
void* list_pop_back(list_t*);
void* list_pop_front(list_t*);
void list_push_back(list_t*, void*);
void list_push_front(list_t*, void*);
void list_remove(list_t*, void*);
