//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_PAGER_H
#define CPPDB_PAGER_H

#include <vector>
#include <fstream>
#include <string>
#include "../Value/value.h"
#include "../BTree/btree.h"
#include "../db_constants.h"

class BTree;
class BTreeNode;

class Pager {
    friend class Table;
    friend class BTree;
    friend class BTreeNode;
private:
    explicit Pager(const std::string& file_name);
    void set_t();

    void write_header_data();
    std::vector<std::string> get_column_data();

    bool read_next_page();
    void insert_row(std::istream& repl);
    void delete_rows(int, const Value&);
    void delete_rows_from_page(int, const Value&);
    void update_rows(int, const Value&, int, const Value&);
    void update_rows_from_page(int, const Value&, int, const Value&);
    std::vector<std::vector<Value>> select_rows(int, const Value&);
    std::vector<std::vector<Value>> select_all_rows();
    std::vector<std::vector<Value>> select_rows_from_page(int, const Value&);
    std::vector<std::vector<Value>> get_page_rows();

    bool read_page_at_pos(int pos);

    void init_btree();
    void set_btree_root(int node_pos);
    BTreeNode read_node(int node_pos);
    BTreeNode insert_new_node();
    void write_node_data(const BTreeNode&);
    void remove_node(int node_pos);

    void dfs() { btree.dfs(*this); }
private:
    int t;
    BTree btree;
    int root_pos;
    int primary_key_pos;
    bool has_primary_key;
    int num_valid_pages;
    int num_pages;
    std::string file_name;
    std::fstream::pos_type page_pos;
    std::vector<std::string> column_names;
    std::vector<std::string> column_types;
    std::vector<std::size_t> column_sizes;
    char page_data[page_size];
    std::size_t row_size;
};

#endif //CPPDB_PAGER_H
