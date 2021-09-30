//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_PTR_H
#define CPPDB_PTR_H

template <class T>
class Ptr {
public:
    void make_unique();

    Ptr();
    Ptr(T* t);
    Ptr(const Ptr& h);
    Ptr& operator=(const Ptr&);
    ~Ptr();

    operator bool() const { return p; }
    T& operator*() const;
    T* operator->() const;

private:
    std::size_t* refptr;
    T* p;
};

#include "Ptr.cpp"

#endif //CPPDB_PTR_H
