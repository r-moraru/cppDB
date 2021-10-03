//
// Created by radua on 9/29/2021.
//

#include <filesystem>
#include <fstream>
#include <iostream>
#include "Table.h"

bool open_file(std::fstream& file, const std::string& file_name) {
    if (std::filesystem::exists(file_name)) {
        std::clog << "Table already exists\n";
        return false;
    }

    file.open(file_name,
              std::ios::binary | std::ios::out);

    return true;
}

void write_empty_page(std::fstream& file) {
    std::string empty_page(page_size, 0);
    file.write(empty_page.c_str(), empty_page.size());
    file.seekg(0);
}

void write_char_size(std::fstream& file, std::string column_dtype) {
    std::string::size_type pos = column_dtype.find('(')+1;

    std::string arr_size = std::string(column_dtype.begin()+pos,
                             column_dtype.begin()+column_dtype.find(')'));

    std::istringstream iss(arr_size);

    int arr_size_i;
    iss >> arr_size_i;

    file.write((char *)&arr_size_i, sizeof(int));
}

void create_table(const std::string& table_name, std::istream& repl) {
    // initialize table
    using std::string;

    std::fstream file;
    if (!open_file(file, table_name+".tab"))
        return;

    write_empty_page(file);

    string column_name, column_dtype;

    // initialize columns
    while (column_name.find(';') == string::npos &&
           column_dtype.find(';') == string::npos) {
        repl >> column_name;
        if (column_name.find(';') != string::npos) {
            std::clog << "Ignored invalid column\n";
            continue;
        }
        repl >> column_dtype;

        if (string("icf").find(column_dtype[0]) == string::npos) {
            std::clog << "Ignored invalid column\n";
            continue;
        }

        file.write(column_name.c_str(), column_name.size()+1);
        file.write(column_dtype.c_str(), 1);

        if (column_dtype[0] == 'c') {
            write_char_size(file, column_dtype);
        }
    }

    file.close();
}

void delete_table(const std::string& table_name) {
    std::string file_name = table_name+".tab";
    std::filesystem::remove(file_name);
}

Table::Table(const std::string& table_name): pager(table_name+".tab")  {
    n = table_name;

    std::vector<std::string> column_data = pager.get_column_data();

    for (auto i = column_data.begin(); i != column_data.end(); i++) {
        pager.column_names.push_back(*i++);
        pager.column_types.push_back(*i++);

        std::size_t column_size;
        std::istringstream iss(*i);
        iss >> column_size;

        pager.column_sizes.push_back(column_size);
        pager.row_size += column_size;
    }
}

using namespace std;

void Table::print() {
    cout << "Table " << n << endl;

    cout << "row size: " << pager.row_size << endl;

    cout << "columns:\n";

    for (int i = 0; i < pager.column_names.size(); i++) {
        cout << pager.column_names[i] << "\t" << pager.column_types[i] << "\t" << pager.column_sizes[i] << endl;
    }
}

std::istream& Table::insert_row(std::istream& repl) {
    pager.insert_row(repl);
    return repl;
}

int get_column_position(const string& column_name,
                        const vector<string>& column_names) {
    for (int i = 0; i < column_names.size(); i++) {
        if (column_name == column_names[i])
            return i;
    }
    return -1;
}

std::ostream& Table::select_rows(std::ostream& os, std::istream& is) {
    std::string column_name;
    is >> column_name;

    int column_pos = get_column_position(column_name, pager.column_names);

    if (column_pos == -1) {
        clog << "Could not find requested column\n";
        return os;
    }

    Value val(pager.column_types[column_pos], pager.column_sizes[column_pos]);
    is >> val;

    vector<vector<Value>> selected_rows = pager.select_rows(column_pos, val);

    os << "Table " << n << "\n";
    for (auto& row : selected_rows) {
        for (auto& v : row) {
            os << v << " ";
        }
        os << "\n";
    }

    return os;
}

std::istream& Table::update_rows(std::istream& is) {

    return is;
}

std::istream& Table::delete_rows(std::istream& is) {

    return is;
}