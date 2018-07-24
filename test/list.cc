#include <assert.h>
#include <list.h>
#include <stdio.h>

void test_list()
{
    List<unsigned> list;

    list = List<unsigned>();
    list.push_front(1);
    assert(list.pop_front() == 1);

    list = List<unsigned>();
    list.push_front(1);
    assert(list.pop_back() == 1);

    list = List<unsigned>();
    list.push_back(1);
    assert(list.pop_front() == 1);

    list = List<unsigned>();
    list.push_back(1);
    assert(list.pop_back() == 1);

    list = List<unsigned>();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    assert(list.pop_front() == 1);
    assert(list.pop_back() == 3);

    list = List<unsigned>();
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    assert(list.pop_back() == 1);
    assert(list.pop_front() == 3);

    list = List<unsigned>();
    assert(list.size() == 0);
    list.push_back(1);
    assert(list.size() == 1);
    list.push_back(2);
    list.push_back(3);
    assert(list.size() == 3);
    list.pop_back();
    list.pop_back();
    assert(list.size() == 1);
    list.pop_back();
    assert(list.size() == 0);

    list = List<unsigned>();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    list.pop_front();
    list.push_back(1);
    assert(list.size() == 3);

    list = List<unsigned>();
    list.push_front(1);
    list.push_front(2);
    list.push_front(3);
    list.pop_back();
    list.push_front(1);
    assert(list.size() == 3);

    list = List<unsigned>();
    list.push_back(1);
    list.push_back(2);
    list.push_back(3);
    assert(list.begin() == list.begin());
    assert(list.begin() != list.end());
    ListIterator<unsigned> iter = list.begin();
    assert(*iter == 1);
    assert(*(iter++) == 1);
    assert(*(++iter) == 3);
    iter++;
    assert(iter == list.end());
}
