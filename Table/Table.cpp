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

void create_table(std::istream& repl) {
    // initialize table
    using std::string;
    string table_name;

    repl >> table_name;

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

Table::Table(const std::string& table_name): pager(table_name+".tab")  {
    row_size = 0;
    name = table_name;
    std::cout << "Reading from table: " << name << std::endl;

    std::vector<std::string> column_data = pager.get_column_data();


    for (auto i = column_data.begin(); i != column_data.end(); i++) {
        column_names.push_back(*i++);
        column_types.push_back(*i++);

        std::size_t column_size;
        std::istringstream iss(*i);
        iss >> column_size;

        column_sizes.push_back(column_size);
        row_size += column_size;
    }
}

using namespace std;

void Table::print() {
    cout << "Table " << name << endl;

    cout << "row size: " << row_size << endl;

    cout << "columns:\n";

    for (int i = 0; i < column_names.size(); i++) {
        cout << column_names[i] << "\t" << column_types[i] << "\t" << column_sizes[i] << endl;
    }
}