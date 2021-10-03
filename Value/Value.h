//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_VALUE_H
#define CPPDB_VALUE_H

#include <string>
#include "String.h"
#include "Float.h"
#include "Int.h"
#include "../Ptr/Ptr.h"

class Value {
    friend std::istream& operator>>(std::istream&, Value&);
    friend std::ostream& operator<<(std::ostream&, Value&);
    friend bool operator==(const Value&, const Value&);
    friend void read_from_buffer(Value&, char *, std::size_t);
    friend void write_to_buffer(const Value&, char *, std::size_t);
public:
    Value(const std::string&, std::size_t);
private:
    Ptr<Base> value_ptr;
};

bool operator==(const Value&, const Value&);
std::istream& operator>>(std::istream& is, Value& value);
std::ostream& operator<<(std::ostream& os, Value& value);
void write_to_buffer(const Value& val, char *buffer, std::size_t);
void read_from_buffer(Value&, char *, std::size_t);

#endif //CPPDB_VALUE_H
