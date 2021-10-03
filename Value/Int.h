//
// Created by radua on 9/30/2021.
//

#ifndef CPPDB_INT_H
#define CPPDB_INT_H

#include <cstring>
#include "Base.h"

class Int: public Base {
    bool operator==(const Base& b) override  {
        const Int& f = dynamic_cast<const Int&>(b);
        return val == f.val;
    }
    void read(std::istream &is) override { is >> val; }
    void write(std::ostream &os) override { os << val; }
    void read_from_buffer(char *buffer, std::size_t) override {
        val = *(int *)buffer;
    }
    void write_to_buffer(char *buffer, std::size_t size) override {
        strncpy(buffer, (char*)&val, size);
    }
private:
    int val;
};

#endif //CPPDB_INT_H
