#include <assert.h>
#include <list.h>
#include <stdlib.h>

void clear(list_t* list)
{
    assert(list);

    while (list->front) {
        pop_front(list);
    }
}

unsigned count(list_t* list)
{
    assert(list);

    list_item_t* list_item = list->front;
    unsigned count = 0;

    while (list_item != 0) {
        list_item = list_item->next;
        ++count;
    }

    return count;
}

void push_back(list_t* list, void* item)
{
    assert(list);

    list_item_t* list_item = malloc(sizeof(list_item_t));
    list_item->item = item;
    list_item->prev = list->back;
    list_item->next = 0;

    if (list->back) {
        list->back->next = list_item;
    } else {
        list->front = list_item;
    }

    list->back = list_item;
}

void push_front(list_t* list, void* item)
{
    assert(list);

    list_item_t* list_item = malloc(sizeof(list_item_t));
    list_item->item = item;
    list_item->prev = 0;
    list_item->next = list->front;

    if (list->front) {
        list->front->prev = list_item;
    } else {
        list->back = list_item;
    }

    list->front = list_item;
}

void* pop_back(list_t* list)
{
    assert(list);
    assert(list->back);

    list_item_t* list_item = list->back;
    void* item = list_item->item;

    list->back = list_item->prev;

    if (list->back) {
        list->back->next = 0;
    } else {
        list->front = 0;
    }

    free(list_item);

    return item;
}

void* pop_front(list_t* list)
{
    assert(list);
    assert(list->front);

    list_item_t* list_item = list->front;
    void* item = list_item->item;

    list->front = list_item->next;

    if (list->front) {
        list->front->prev = 0;
    } else {
        list->back = 0;
    }

    free(list_item);

    return item;
}

void remove(list_t* list, void* item)
{
    assert(list);

    list_item_t* list_item = list->front;

    while (list_item != 0) {
        list_item_t* next = list_item->next;

        if (list_item->item == item) {
            if (list_item == list->front) {
                pop_front(list);
            } else if (list_item == list->back) {
                pop_back(list);
            } else {
                list_item->prev->next = list_item->next;
                list_item->next->prev = list_item->prev;

                free(list_item);
            }
        }

        list_item = next;
    }
}
