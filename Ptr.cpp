//
// Created by radua on 9/29/2021.
//

#include <cstdlib>
#include <stdexcept>
#include "Ptr.h"

template <class T>
Ptr<T>::Ptr(): p(nullptr), refptr(new std::size_t(1)) { }

template <class T>
Ptr<T>::Ptr(T* t): p(t), refptr(new std::size_t(1)) { }

template <class T>
Ptr<T>::Ptr(const Ptr& h): p(h.p), refptr(h.refptr) { ++*refptr; }

template <class T>
T& Ptr<T>::operator*() const {
    if (p)
        return *p;
    throw std::runtime_error("unbound Ptr");
}

template <class T>
T* Ptr<T>::operator->() const {
    if (p)
        return p;
    throw std::runtime_error("unbound Ptr");
}

template <class T>
Ptr<T>& Ptr<T>::operator=(const Ptr& rhs) {
    ++*rhs.refptr;

    if (--*refptr == 0) {
        delete refptr;
        delete p;
    }

    refptr = rhs.refptr;
    p = rhs.p;
    return *this;
}

template <class T>
Ptr<T>::~Ptr() {
    if (--*refptr == 0) {
        delete refptr;
        delete p;
    }
}

template <class T>
T* clone(const T* tp) {
    return tp->clone();
}

template <class T>
void Ptr<T>::make_unique() {
    if (*refptr != 1) {
        --*refptr;
        refptr = new std::size_t(1);
        p = p ? clone(p) : nullptr;
    }
}
