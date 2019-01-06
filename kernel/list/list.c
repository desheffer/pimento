#include <assert.h>
#include <kstdlib.h>
#include <list.h>

void list_clear(struct list * list)
{
    assert(list != 0);

    while (list->front) {
        list_pop_front(list);
    }
}

unsigned list_count(struct list * list)
{
    assert(list != 0);

    struct list_item * list_item = list->front;
    unsigned count = 0;

    while (list_item != 0) {
        ++count;
        list_item = list_item->next;
    }

    return count;
}

struct list * list_create(void)
{
    struct list * list = kzalloc(sizeof(struct list));

    return list;
}

void list_destroy(struct list * list)
{
    assert(list != 0);

    list_clear(list);
    kfree(list);
}

void list_push_back(struct list * list, void * item)
{
    assert(list != 0);

    struct list_item * list_item = kzalloc(sizeof(struct list_item));
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

void list_push_front(struct list * list, void * item)
{
    assert(list != 0);

    struct list_item * list_item = kzalloc(sizeof(struct list_item));
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

void * list_pop_back(struct list * list)
{
    assert(list != 0);
    assert(list->back != 0);

    struct list_item * list_item = list->back;
    void * item = list_item->item;

    list->back = list_item->prev;

    if (list->back) {
        list->back->next = 0;
    } else {
        list->front = 0;
    }

    kfree(list_item);

    return item;
}

void * list_pop_front(struct list * list)
{
    assert(list != 0);
    assert(list->front != 0);

    struct list_item * list_item = list->front;
    void * item = list_item->item;

    list->front = list_item->next;

    if (list->front) {
        list->front->prev = 0;
    } else {
        list->back = 0;
    }

    kfree(list_item);

    return item;
}

void list_remove(struct list * list, void * item)
{
    assert(list != 0);

    struct list_item * list_item = list->front;

    while (list_item != 0) {
        struct list_item * next = list_item->next;

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
