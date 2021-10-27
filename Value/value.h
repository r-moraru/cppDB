//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_VALUE_H
#define CPPDB_VALUE_H

#include <string>
#include "string_val.h"
#include "float.h"
#include "int.h"
#include "../Ptr/ptr.h"

class Value {
    friend std::istream& operator>>(std::istream&, const Value&);
    friend std::ostream& operator<<(std::ostream&, const Value&);
    friend bool operator==(const Value&, const Value&);
    friend bool operator<(const Value&, const Value&);
    friend void read_from_buffer(Value&, char *, std::size_t);
    friend void write_to_buffer(const Value&, char *, std::size_t);
public:
    Value() : value_ptr(nullptr) { };
    Value(const std::string&, std::size_t);
private:
    Ptr<Base> value_ptr;
};

bool operator<(const Value&, const Value&);
bool operator==(const Value&, const Value&);
std::istream& operator>>(std::istream& is, const Value& value);
std::ostream& operator<<(std::ostream& os, const Value& value);
void write_to_buffer(const Value& val, char *buffer, std::size_t);
void read_from_buffer(Value&, char *, std::size_t);

#endif //CPPDB_VALUE_H
