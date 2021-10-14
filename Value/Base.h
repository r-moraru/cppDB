//
// Created by radua on 9/30/2021.
//

#ifndef CPPDB_BASE_H
#define CPPDB_BASE_H


#include <istream>

class Value;

class Base {
    friend bool operator==(const Value&, const Value&);
    friend bool operator<(const Value& val1, const Value& val2);
    friend std::istream& operator>>(std::istream&, const Value&);
    friend std::ostream& operator<<(std::ostream&, const Value&);
    friend void read_from_buffer(Value&, char *, std::size_t);
    friend void write_to_buffer(const Value&, char *, std::size_t);
    friend class Float;
    friend class Int;
    friend class String;

    virtual void read(std::istream&) = 0;
    virtual void write(std::ostream&) = 0;
    virtual void write_to_buffer(char *, std::size_t) = 0;
    virtual void read_from_buffer(char *, std::size_t) = 0;
    virtual bool operator==(const Base&) = 0;
    virtual bool operator<(const Base&) = 0;
public:
    virtual ~Base() { }
};


#endif //CPPDB_BASE_H
