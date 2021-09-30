//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_TABLE_H
#define CPPDB_TABLE_H

#include "../constants.h"

#include "../Value/Value.h"
#include "../Ptr/Ptr.h"
#include "../Row/Row.h"
#include "../Pager/Pager.h"
#include <string>
#include <vector>

class Table {
public:
    class iterator;
    explicit Table(const std::string&);  // create table object from existing table file

    void print();
    std::ostream& select_rows(std::ostream&, std::string column, Value val);
    std::istream& insert_rows(std::istream&);
    void delete_rows(std::string column, Value val);
    void replace_rows(std::string column, Value old_val, Value new_val);

    iterator& begin();
    iterator& end();
private:
    Pager pager;
    std::string name;
    std::size_t row_size;
    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::vector<std::size_t> column_sizes;
};

void create_table(std::istream& repl);

class Table::iterator {
    iterator& operator++(); // get next row
    Row& operator*();
    Ptr<Row>& operator->();
    bool operator==(iterator iter);
private:
    Pager pager;
    Ptr<Row> pr;
};


#endif //CPPDB_TABLE_H
