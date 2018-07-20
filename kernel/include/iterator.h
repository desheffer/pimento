#pragma once

template <class T>
class Iterator
{
  public:
    Iterator<T>& operator++();
    Iterator<T> operator++(int);
    bool operator==(const Iterator<T>) const;
    bool operator!=(const Iterator<T>) const;
    T operator*() const;
};
