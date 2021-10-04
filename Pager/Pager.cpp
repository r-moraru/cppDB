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

    file.close();
}

vector<string> Pager::get_column_data() {
    vector<string> column_data;

    char* iter1 = page_data;
    char* iter2 = strchr(page_data, '\0');

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
    char row_data[row_size];
    memset(row_data, 0, row_size);
    char *iter = row_data;

    for (int i = 0; i < column_names.size(); i++) {
        Value val(column_types[i], column_sizes[i]);
        // should call virtual function
        repl >> val;
        write_to_buffer(val, iter, column_sizes[i]);

        iter += column_sizes[i];
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

    vector<vector<Value>> page_rows = get_page_rows();

    for (auto& row : page_rows)
        if (row[pos] == val) ret.push_back(row);

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
            std::cout << "Found a matching row:\n";
            for (auto& val : page_rows[i]) {
                std::cout << val << " ";
            }
            std::cout << std::endl;

            std::size_t pos_in_row = 0;
            for (int j = 0; j < pos; j++) pos_in_row += column_sizes[j];
            std::cout << "Position in row: " << pos_in_row << std::endl;

            std::size_t pos_in_page = sizeof(int) + i*row_size + pos_in_row;
            std::cout << "Position in page: " << pos_in_page << std::endl;

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