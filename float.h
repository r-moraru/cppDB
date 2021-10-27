//
// Created by radua on 9/30/2021.
//

#ifndef CPPDB_FLOAT_H
#define CPPDB_FLOAT_H

#include <cstring>
#include "base.h"

class Float: public Base {
    bool operator==(const Base& b) override {
        const Float& f = dynamic_cast<const Float&>(b);
        return val == f.val;
    }
    bool operator<(const Base& b) override {
        const Float& f = dynamic_cast<const Float&>(b);
        return val < f.val;
    }
    void read(std::istream &is) override { is >> val; }
    void write(std::ostream &os) override { os << val; }
    void read_from_buffer(char *buffer, std::size_t) override {
        val = *(float *)buffer;
    }
    void write_to_buffer(char *buffer, std::size_t size) override {
        memcpy(buffer, (char*)&val, size);
    }
private:
    float val;
};

#endif //CPPDB_FLOAT_H
