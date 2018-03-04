#pragma once

#include <assert.h>
#include <iterator.h>

template <class T>
class ListItem
{
  public:
    ListItem(T item) : _item(item) {}
    ~ListItem() {}
    ListItem<T>* prev() const { return _prev; }
    ListItem<T>* next() const { return _next; }
    T item() const { return _item; }
    void setPrev(ListItem<T>* prev) { _prev = prev; }
    void setNext(ListItem<T>* next) { _next = next; }
  private:
    const T _item;
    ListItem<T>* _prev = 0;
    ListItem<T>* _next = 0;
};

template <class T>
class ListIterator : public Iterator<T>
{
  public:
    ListIterator(ListItem<T>* listItem) { _listItem = listItem; }
    ListIterator(const ListIterator<T>& that) { _listItem = that._listItem; }
    ~ListIterator() {}
    ListIterator<T>& operator++();
    ListIterator<T> operator++(int);
    bool operator==(const ListIterator<T> that) const { return _listItem == that._listItem; }
    bool operator!=(const ListIterator<T> that) const { return _listItem != that._listItem; }
    T operator*() const { return _listItem->item(); }
  private:
    ListItem<T>* _listItem;
};

template <class T>
class List
{
  public:
    List() {}
    ~List();
    void push_back(T);
    void push_front(T);
    T pop_back();
    T pop_front();
    unsigned size() const;
    ListIterator<T> begin() const { return ListIterator<T>(_front); }
    ListIterator<T> end() const { return ListIterator<T>(0); }
  private:
    ListItem<T>* _front = 0;
    ListItem<T>* _back = 0;
};

template <class T>
ListIterator<T>& ListIterator<T>::operator++()
{
    _listItem = _listItem->next();
    return *this;
}

template <class T>
ListIterator<T> ListIterator<T>::operator++(int)
{
    ListIterator<T> original(*this);
    ++(*this);
    return original;
}

template <class T>
List<T>::~List()
{
    while (_front) {
        auto next = _front->next();
        delete _front;
        _front = next;
    }
}

template <class T>
void List<T>::push_back(T item)
{
    auto listItem = new ListItem<T>(item);
    listItem->setPrev(_back);

    if (_back) {
        _back->setNext(listItem);
    } else {
        _front = listItem;
    }

    _back = listItem;
}

template <class T>
void List<T>::push_front(T item)
{
    auto listItem = new ListItem<T>(item);
    listItem->setNext(_front);

    if (_front) {
        _front->setPrev(listItem);
    } else {
        _back = listItem;
    }

    _front = listItem;
}

template <class T>
T List<T>::pop_back()
{
    assert(_back);

    auto listItem = _back;
    auto item = listItem->item();

    _back = listItem->prev();

    if (_back) {
        _back->setNext(0);
    } else {
        _front = 0;
    }

    delete listItem;

    return item;
}

template <class T>
T List<T>::pop_front()
{
    assert(_front);

    auto listItem = _front;
    auto item = listItem->item();

    _front = listItem->next();

    if (_front) {
        _front->setNext(0);
    } else {
        _back = 0;
    }

    delete listItem;

    return item;
}

template <class T>
unsigned List<T>::size() const
{
    unsigned count = 0;
    for (auto iter = begin(); iter != end(); ++iter) {
        ++count;
    }

    return count;
}
