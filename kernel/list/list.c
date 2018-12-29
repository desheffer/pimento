#include <assert.h>
#include <kstdlib.h>
#include <list.h>

void list_clear(list_t* list)
{
    assert(list != 0);

    while (list->front) {
        list_pop_front(list);
    }
}

unsigned list_count(list_t* list)
{
    assert(list != 0);

    list_item_t* list_item = list->front;
    unsigned count = 0;

    while (list_item != 0) {
        list_item = list_item->next;
        ++count;
    }

    return count;
}

void list_delete(list_t* list)
{
    assert(list != 0);

    list_clear(list);
    kfree(list);
}

list_t* list_new(void)
{
    list_t* list = kzalloc(sizeof(list_t));

    return list;
}

void list_push_back(list_t* list, void* item)
{
    assert(list != 0);

    list_item_t* list_item = kzalloc(sizeof(list_item_t));
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

void list_push_front(list_t* list, void* item)
{
    assert(list != 0);

    list_item_t* list_item = kzalloc(sizeof(list_item_t));
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

void* list_pop_back(list_t* list)
{
    assert(list != 0);
    assert(list->back != 0);

    list_item_t* list_item = list->back;
    void* item = list_item->item;

    list->back = list_item->prev;

    if (list->back) {
        list->back->next = 0;
    } else {
        list->front = 0;
    }

    kfree(list_item);

    return item;
}

void* list_pop_front(list_t* list)
{
    assert(list != 0);
    assert(list->front != 0);

    list_item_t* list_item = list->front;
    void* item = list_item->item;

    list->front = list_item->next;

    if (list->front) {
        list->front->prev = 0;
    } else {
        list->back = 0;
    }

    kfree(list_item);

    return item;
}

void list_remove(list_t* list, void* item)
{
    assert(list != 0);

    list_item_t* list_item = list->front;

    while (list_item != 0) {
        list_item_t* next = list_item->next;

        if (list_item->item == item) {
            if (list_item == list->front) {
                list_pop_front(list);
            } else if (list_item == list->back) {
                list_pop_back(list);
            } else {
                list_item->prev->next = list_item->next;
                list_item->next->prev = list_item->prev;

                kfree(list_item);
            }
        }

        list_item = next;
    }
}
