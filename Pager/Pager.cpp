//
// Created by radua on 9/29/2021.
//

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include "Pager.h"

using std::vector;
using std::string;
using std::sort;

Pager::Pager(const string& file_name): file_name(file_name),
                                            page_data{0}, row_size(0) {
    std::fstream file(file_name,
                      std::ios::binary |
                      std::ios::in);

    page_pos = file.tellg();

    file.read(page_data, page_size);

    char *iter = page_data;
    has_primary_key = *(bool *)iter;
    iter += sizeof(has_primary_key);

    primary_key_pos = *(int *)iter;
    iter += sizeof(primary_key_pos);

    root_pos = *(int *)iter;
    iter += sizeof(root_pos);

    num_valid_pages = *(int *)iter;

    file.seekg(std::ios::end);
    num_pages = file.tellg() / page_size;

    file.close();
}

void Pager::set_t() {
    int key_value_size = column_sizes[primary_key_pos] + row_size;
    int empty_room = page_size-BTREENODE_HEADER_SIZE;

    t = (empty_room+key_value_size)/(2*key_value_size);

    key_value_size = column_sizes[primary_key_pos] + sizeof(int);

    t = std::min(t, (int)(empty_room-sizeof(int)+key_value_size)/(2*key_value_size));
}

vector<string> Pager::get_column_data() {
    vector<string> column_data;

    char* iter1 = page_data + HEADER_SIZE;
    char* iter2 = strchr(iter1, '\0');

    while (iter2 != nullptr && iter2-page_data <= page_size) {
        if (*iter1 == 0)
            break;

        string column_name = string(iter1, iter2);
        column_data.emplace_back(iter1, iter2);

        // move iterator to first char after column n
        iter1 = iter2+1;

        // read column type
        string column_type = string(iter1, iter1+1);

        // move iterator to next byte
        iter1++;
        iter2 = iter1;

        column_data.push_back(column_type);
        int column_size;

        if (column_type == "c") {
            column_size = *(int *)iter1;

            iter2 += sizeof(int);
            iter1 = iter2;
        }
        else if (column_type == "i")
            column_size = sizeof(int);
        else
            column_size = sizeof(float);

        column_data.push_back(std::to_string(column_size));

        // get next column n
        iter2 = strchr(iter1, '\0');
    }

    *(int *)page_data = page_size;

    return column_data;
}

bool Pager::read_next_page() {
    std::fstream file(file_name,
                      std::ios::binary |
                      std::ios::in);

    page_pos += page_size;

    file.seekg(page_pos);

    if (file.read(page_data, page_size)) {
        file.close();
        return true;
    }

    page_pos -= page_size;
    file.close();

    return false;
}

void Pager::read_page_at_pos(int pos) {
    std::fstream file(file_name,
                      std::ios::binary |
                      std::ios::in);

    file.seekg(pos);

    file.read(page_data, page_size);
}

bool insert_row_in_page(char* page_data, const char* row_data, std::size_t row_size) {
    int first_bit = *(int *)page_data;
    std::clog << "inserting row at position " << first_bit << " in page\n";
    if (first_bit + row_size <= page_size) {
        std::clog << "can insert row\n";
        memcpy(page_data+first_bit, row_data, row_size);
        *(int *)page_data = first_bit + row_size;
        return true;
    }
    return false;
}

void write_page(const string& file_name,
                const char* page_data, std::streampos page_pos) {
    std::fstream file(file_name,
                      std::ios::binary |
                      std::ios::out |
                      std::ios::in);

    std::clog << "Writing page at pos " << page_pos << std::endl;
    file.seekg(page_pos);

    file.write(page_data, page_size);

    file.close();
}

void append_page(const string& file_name) {
    std::fstream file(file_name,
                      std::ios::binary |
                      std::ios::out |
                      std::ios::app);

    file.seekg(0, std::fstream::end);

    std::clog << "Writing new page at position " << file.tellg() << std::endl;

    char empty_page[page_size] = {0};
    *(int *)empty_page = sizeof(int);
    std::clog << "New page with first row going at: " << *(int *)empty_page << std::endl;
    file.write(empty_page, page_size);

    file.close();
}

void Pager::insert_row(std::istream& repl) {
    std::vector<Value> row;
    char row_data[row_size];
    memset(row_data, 0, row_size);

    char *iter = row_data;
    for (int i = 0; i < column_names.size(); i++) {
        Value val(column_types[i], column_sizes[i]);
        repl >> val;

        write_to_buffer(val, iter, column_sizes[i]);
        iter += column_sizes[i];

        row.push_back(val);
    }

    if (has_primary_key) {
        btree.insert(*this, row[primary_key_pos], row);
        // TODO: otherwise traverse leaf nodes
        return;
    }

    while (!insert_row_in_page(page_data, row_data, row_size)) {
        if (!read_next_page()) {
            std::clog << "Could not fit row in existing page, creating new one\n";
            append_page(file_name);
            read_next_page();
            insert_row_in_page(page_data, row_data, row_size);
            break;
        }
    }

    std::clog << "Writing at page_pos: " << page_pos << std::endl;

    write_page(file_name, page_data, page_pos);
}

vector<vector<Value>> Pager::select_rows_from_page(int pos, const Value& val) {
    vector<vector<Value>> ret;

    if (!has_primary_key) {
        vector<vector<Value>> page_rows = get_page_rows();

        for (auto& row : page_rows)
            if (row[pos] == val) ret.push_back(row);

        return ret;
    }

    if (pos == primary_key_pos) {
        vector<Value> row = btree.get_data(*this, val);
        if (row.size() > 0)
            ret.push_back(row);

        return ret;
    }

    // TODO: otherwise, traverse btree leafs
    return ret;
}

vector<vector<Value>> Pager::select_rows(int column_position, const Value& val) {
    vector<vector<Value>> selected_rows;

    while (read_next_page()) {
        std::clog << "currently at page " << page_pos/page_size << std::endl;
        vector<vector<Value>> new_selected_rows =
                select_rows_from_page(column_position, val);

        std::copy(new_selected_rows.begin(), new_selected_rows.end(),
                  std::back_inserter(selected_rows));
    }

    return selected_rows;
}

std::vector<std::vector<Value>> Pager::get_page_rows() {
    std::vector<std::vector<Value>> ret;

    std::size_t iter = sizeof(int);
    std::size_t end = *(int *)page_data;

    for (iter = sizeof(int); iter < end; ) {
        std::vector<Value> row;
        for (int i = 0; i < column_names.size(); i++) {
            Value val(column_types[i], column_sizes[i]);
            // values should have a value size member in order to avoid passing it as arg
            read_from_buffer(val, page_data+iter, column_sizes[i]);

            row.push_back(val);

            iter += column_sizes[i];
        }

        ret.push_back(row);
    }

    return ret;
}

void Pager::delete_rows_from_page(int pos, const Value& val) {
    vector<int> deleted_pos;

    vector<vector<Value>> page_rows = get_page_rows();

    for (int i = 0; i < page_rows.size(); i++) {
        if (page_rows[i][pos] == val) deleted_pos.push_back(i);
    }

    sort(deleted_pos.begin(), deleted_pos.end(), std::greater<>());

    for (auto& i : deleted_pos) {
        std::clog << "Deleting row at position " << i << std::endl;
        memcpy(page_data+sizeof(int)+row_size*i,
               page_data+*(int *)page_data-row_size,
               row_size);

        *(int *)page_data -= (int)row_size;
    }

    // remove artifacts
    std::size_t iter = *(int *)page_data;
    memset(page_data+iter, 0, row_size*deleted_pos.size());

    if (!deleted_pos.empty())
        write_page(file_name, page_data, page_pos);
}

void Pager::delete_rows(int column_position, const Value& val) {
    while (read_next_page()) {
        delete_rows_from_page(column_position, val);
    }
}

void Pager::update_rows_from_page(int pos, const Value& old_val, const Value& new_val) {
    bool updated_rows = false;

    vector<vector<Value>> page_rows = get_page_rows();

    for (int i = 0; i < page_rows.size(); i++) {
        if (page_rows[i][pos] == old_val) {
            updated_rows = true;

            std::size_t pos_in_row = 0;
            for (int j = 0; j < pos; j++) pos_in_row += column_sizes[j];

            std::size_t pos_in_page = sizeof(int) + i*row_size + pos_in_row;
            write_to_buffer(new_val, page_data+pos_in_page, column_sizes[pos]);
        }
    }

    if (updated_rows)
        write_page(file_name, page_data, page_pos);
}

void Pager::update_rows(int column_position, const Value& old_value, const Value& new_value) {
    while (read_next_page()) {
        update_rows_from_page(column_position, old_value, new_value);
    }
}

void Pager::init_btree() {
    btree.root = read_node(root_pos);
}

BTreeNode Pager::insert_new_node() {
    BTreeNode empty_node(t);
    if (num_valid_pages == num_pages) {
        append_page(file_name);
        num_pages++;
    }

    empty_node.pos = num_valid_pages;
    write_node_data(empty_node);

    num_valid_pages++;

    return empty_node;
}

BTreeNode Pager::read_node(int node_pos) {
    BTreeNode node(t);
    node.pos = node_pos;

    read_page_at_pos(node_pos);

    char *iter = page_data;

    node.is_leaf = *(bool *)iter;
    iter += sizeof(node.is_leaf);

    node.n = *(int *)iter;
    iter += sizeof(node.n);

    node.pos = *(int *)iter;
    iter += sizeof(node.n);

    node.parent_pos = *(int *)iter;
    iter += sizeof(node.parent_pos);

    node.next_leaf = *(int *)iter;
    iter += sizeof(node.next_leaf);

    for (int i = 0; i < node.n; i++) {
        Value val(column_types[primary_key_pos],
                  column_sizes[primary_key_pos]);
        read_from_buffer(val, iter, column_sizes[primary_key_pos]);

        node.keys[i] = val;

        iter += column_sizes[primary_key_pos];
    }
    if (node.is_leaf) {
        for (int i = 0; i < node.n; i++) {
            for (int j = 0; j < column_names.size(); j++) {
                Value val(column_types[j], column_sizes[j]);
                read_from_buffer(val, iter, column_sizes[j]);

                node.data[i].push_back(val);

                iter += column_sizes[j];
            }
        }
    }
    else {
        for (int i = 0; i <= node.n; i++) {
            node.c[i] = *(int *)iter;
            iter += sizeof(node.c[i]);
        }
    }

    return node;
}

void Pager::write_node_data(const BTreeNode &node) {
    char page_buffer[page_size] = {0};

    char *iter = page_buffer;

    *(bool *)iter = node.is_leaf;
    iter += sizeof(node.is_leaf);

    *(int *)iter = node.n;
    iter += sizeof(node.n);

    *(int *)iter = node.pos;
    iter += sizeof(node.pos);

    *(int *)iter = node.parent_pos;
    iter += sizeof(node.parent_pos);

    *(int *)iter = node.next_leaf;
    iter += sizeof (node.next_leaf);

    for (int i = 0; i < node.n; i++) {
        write_to_buffer(node.keys[i], iter,
                        column_sizes[primary_key_pos]);
        iter += column_sizes[primary_key_pos];
    }
    if (node.is_leaf) {
        for (int i = 0; i < node.n; i++) {
            for (int j = 0; j < column_names.size(); j++) {
                write_to_buffer(node.data[i][j], iter, column_sizes[j]);
                iter += column_sizes[j];
            }
        }
    }
    else {
        for (int i = 0; i <= node.n; i++) {
            *(int *)iter = node.c[i];
            iter += sizeof(node.c[i]);
        }
    }

    write_page(file_name, page_buffer, page_size*node.pos);
}

void Pager::remove_node(int node_pos) {
    num_valid_pages--;

    read_page_at_pos(num_valid_pages);

    write_page(file_name, page_data, node_pos);

    if (root_pos == num_valid_pages) {
        root_pos = node_pos;
    }

    BTreeNode moved = read_node(node_pos);
    moved.pos = node_pos;

    if (moved.parent_pos != -1) {
        BTreeNode parent = read_node(moved.parent_pos);
        for (int i = 0; i <= parent.n; i++)
            if (parent.c[i] == num_valid_pages) {
                parent.c[i] = node_pos;
                break;
            }
    }

    for (int i = 0; i <= moved.n; i++) {
        BTreeNode child = read_node(moved.c[i]);
        child.parent_pos = node_pos;
        write_node_data(child);
    }

    if (moved.next_leaf != -1) {
        BTreeNode nl = read_node(moved.next_leaf);
        nl.prev_leaf = node_pos;
        write_node_data(nl);
    }
    if (moved.prev_leaf != -1) {
        BTreeNode pl = read_node(moved.prev_leaf);
        pl.next_leaf = node_pos;
        write_node_data(pl);
    }

    write_node_data(moved);
}

void Pager::set_btree_root(int node_pos) {
    root_pos = node_pos;
}