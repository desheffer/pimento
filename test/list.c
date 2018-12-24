#include <assert.h>
#include <list.h>
#include <stdlib.h>

void test_list()
{
    list_t* list = list_new();

    int one = 1;
    int two = 2;
    int three = 3;
    int four = 4;

    list_push_front(list, &one);
    assert(list_pop_front(list) == &one);
    list_clear(list);

    list_push_front(list, &one);
    assert(list_pop_back(list) == &one);
    list_clear(list);

    list_push_back(list, &one);
    assert(list_pop_front(list) == &one);
    list_clear(list);

    list_push_back(list, &one);
    assert(list_pop_back(list) == &one);
    list_clear(list);

    list_push_back(list, &one);
    list_push_back(list, &two);
    list_push_back(list, &three);
    assert(list_pop_front(list) == &one);
    assert(list_pop_back(list) == &three);
    list_clear(list);

    list_push_front(list, &one);
    list_push_front(list, &two);
    list_push_front(list, &three);
    assert(list_pop_back(list) == &one);
    assert(list_pop_front(list) == &three);
    list_clear(list);

    assert(list_count(list) == 0);
    list_push_back(list, &one);
    assert(list_count(list) == 1);
    list_push_back(list, &two);
    list_push_back(list, &three);
    assert(list_count(list) == 3);
    list_pop_back(list);
    list_pop_back(list);
    assert(list_count(list) == 1);
    list_pop_back(list);
    assert(list_count(list) == 0);
    list_clear(list);

    list_push_back(list, &one);
    list_push_back(list, &two);
    list_push_back(list, &three);
    list_pop_front(list);
    list_push_back(list, &one);
    assert(list_count(list) == 3);
    list_clear(list);

    list_push_front(list, &one);
    list_push_front(list, &two);
    list_push_front(list, &three);
    list_pop_back(list);
    list_push_front(list, &one);
    assert(list_count(list) == 3);
    list_clear(list);

    list_push_back(list, &one);
    list_push_back(list, &two);
    list_push_back(list, &three);
    assert(list_count(list) == 3);
    list_remove(list, &two);
    assert(list_count(list) == 2);
    list_remove(list, &four);
    assert(list_count(list) == 2);
    list_remove(list, &one);
    assert(list_count(list) == 1);
    list_remove(list, &three);
    assert(list_count(list) == 0);
    list_clear(list);

    list_delete(list);
}
