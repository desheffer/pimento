#include <kstdlib.h>
#include <list.h>

/**
 * Create a list.
 */
struct list * list_create(void)
{
    struct list * list = kcalloc(sizeof(struct list));

    return list;
}

/**
 * Destroy a list.
 */
void list_destroy(struct list * list)
{
    list_clear(list);
    kfree(list);
}

/**
 * Clear all items in a list.
 */
void list_clear(struct list * list)
{
    while (list->front) {
        list_pop_front(list);
    }
}

/**
 * Calculate the number of items in a list.
 */
unsigned list_count(struct list * list)
{
    struct list_item * list_item = list->front;
    unsigned count = 0;

    while (list_item != 0) {
        ++count;
        list_item = list_item->next;
    }

    return count;
}

/**
 * Add an item to the end of the list.
 */
void list_push_back(struct list * list, void * item)
{
    struct list_item * list_item = kcalloc(sizeof(struct list_item));
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

/**
 * Add an item to the beginning of a list.
 */
void list_push_front(struct list * list, void * item)
{
    struct list_item * list_item = kcalloc(sizeof(struct list_item));
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

/**
 * Remove an item from the end of a list.
 */
void * list_pop_back(struct list * list)
{
    if (list->back == 0) {
        return 0;
    }

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

/**
 * Remove an item from the beginning of a list.
 */
void * list_pop_front(struct list * list)
{
    if (list->front == 0) {
        return 0;
    }

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

/**
 * Remove a specific item from a list.
 */
void list_remove(struct list * list, void * item)
{
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
