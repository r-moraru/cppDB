//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_PAGER_H
#define CPPDB_PAGER_H

#include <vector>
#include <fstream>
#include <string>
#include "../Value/Value.h"
#include "../constants.h"

class Pager {
    friend class Table;
private:
    explicit Pager(const std::string& file_name);

    std::vector<std::string> get_column_data();
    bool read_next_page();
    void insert_row(std::istream& repl);
    void delete_rows(int, const Value&);
    void delete_rows_from_page(int, const Value&);
    void update_rows(int, const Value&, const Value&);
    void update_rows_from_page(int, const Value&, const Value&);
    std::vector<std::vector<Value>> select_rows(int, const Value&);
    std::vector<std::vector<Value>> select_rows_from_page(int, const Value&);
    std::vector<std::vector<Value>> get_page_rows();
private:
    std::string file_name;
    char page_data[page_size];
    std::fstream::pos_type page_pos;
    std::size_t row_size;
    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::vector<std::size_t> column_sizes;
};

#endif //CPPDB_PAGER_H
