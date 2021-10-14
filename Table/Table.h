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
    explicit Table(const std::string&);  // create table object from existing table file

    void print();
    void dfs() { pager.dfs(); }
    void add_root_node();
    std::istream& insert_row(std::istream&);
    std::istream& update_rows(std::istream&);
    std::istream& delete_rows(std::istream&);
    std::ostream& select_rows(std::ostream&, std::istream&);

    std::string name() { return n; }
private:
    Pager pager;
    std::string n;
};

bool create_table(const std::string&, std::istream& repl);
void delete_table(const std::string& table_name);

#endif //CPPDB_TABLE_H
