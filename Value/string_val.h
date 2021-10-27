//
// Created by radua on 9/30/2021.
//

#ifndef CPPDB_STRING_H
#define CPPDB_STRING_H

#include <cstring>
#include "base.h"

class String: public Base {
    bool operator==(const Base& b) override {
        const String& s = dynamic_cast<const String&>(b);
        return val == s.val;
    }
    bool operator<(const Base& b) override {
        const String& s = dynamic_cast<const String&>(b);
        return val < s.val;
    }
    void read(std::istream &is) override { is >> val; }
    void write(std::ostream &os) override { os << val; }
    void read_from_buffer(char *buffer, std::size_t sz) override {
        char temp[sz+1];
        memcpy(temp, buffer, sz);
        temp[sz] = '\0';

        val = temp;
    }
    void write_to_buffer(char *buffer, std::size_t size) override {
        memcpy(buffer, val.c_str(), std::min(size, val.size()+1));
    }
private:
    std::string val;
};

#endif //CPPDB_STRING_H
