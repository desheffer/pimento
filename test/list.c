#include <assert.h>
#include <list.h>
#include <stdlib.h>

void test_list()
{
    list_t* list = malloc(sizeof(list_t));
    list->front = 0;
    list->back = 0;

    int one = 1;
    int two = 2;
    int three = 3;
    int four = 4;

    push_front(list, &one);
    assert(pop_front(list) == &one);
    clear(list);

    push_front(list, &one);
    assert(pop_back(list) == &one);
    clear(list);

    push_back(list, &one);
    assert(pop_front(list) == &one);
    clear(list);

    push_back(list, &one);
    assert(pop_back(list) == &one);
    clear(list);

    push_back(list, &one);
    push_back(list, &two);
    push_back(list, &three);
    assert(pop_front(list) == &one);
    assert(pop_back(list) == &three);
    clear(list);

    push_front(list, &one);
    push_front(list, &two);
    push_front(list, &three);
    assert(pop_back(list) == &one);
    assert(pop_front(list) == &three);
    clear(list);

    assert(count(list) == 0);
    push_back(list, &one);
    assert(count(list) == 1);
    push_back(list, &two);
    push_back(list, &three);
    assert(count(list) == 3);
    pop_back(list);
    pop_back(list);
    assert(count(list) == 1);
    pop_back(list);
    assert(count(list) == 0);
    clear(list);

    push_back(list, &one);
    push_back(list, &two);
    push_back(list, &three);
    pop_front(list);
    push_back(list, &one);
    assert(count(list) == 3);
    clear(list);

    push_front(list, &one);
    push_front(list, &two);
    push_front(list, &three);
    pop_back(list);
    push_front(list, &one);
    assert(count(list) == 3);
    clear(list);

    push_back(list, &one);
    push_back(list, &two);
    push_back(list, &three);
    assert(count(list) == 3);
    remove(list, &two);
    assert(count(list) == 2);
    remove(list, &four);
    assert(count(list) == 2);
    remove(list, &one);
    assert(count(list) == 1);
    remove(list, &three);
    assert(count(list) == 0);
    clear(list);
}
