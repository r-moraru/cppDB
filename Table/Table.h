//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_TABLE_H
#define CPPDB_TABLE_H


#include "../Value.h"
#include "../Ptr.h"
#include "../Row/Row.h"
#include "../Pager/Pager.h"
#include <string>
#include <vector>

class Table {
public:
    class iterator;

    std::ostream& select_rows(std::ostream&, std::string column, Value val);
    std::istream& insert_rows(std::istream&);
    void delete_rows(std::string column, Value val);
    void replace_rows(std::string column, Value old_val, Value new_val);
private:
    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::vector<std::string> column_sizes;
    std::size_t row_size;
};

class Table::iterator {
    iterator& operator++(); // get next row
    Row& operator*();
    Ptr<Row>& operator->();
private:
    Pager pager;
    Ptr<Row> pr;
};


#endif //CPPDB_TABLE_H
