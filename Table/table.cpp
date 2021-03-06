//
// Created by radua on 9/29/2021.
//

#include <QMessageBox>

#include <QFile>
#include <QDir>
#include <fstream>
#include <iostream>
#include <sstream>
#include "table.h"

bool open_file(std::fstream& file, const std::string& file_name) {
    QString path = QDir::currentPath() + "/DBs/" + QString::fromStdString(file_name);
    if (QFile(path).exists()) {
        std::clog << "Table already exists\n";
        return false;
    }

    file.open(path.toStdString(),
              std::ios::binary | std::ios::out);

    return true;
}

void write_empty_page(std::fstream& file) {
    std::string empty_page(page_size, 0);
    file.write(empty_page.c_str(), empty_page.size());
    file.seekg(0);
}

void write_char_size(char *page_buffer, std::string column_dtype) {
    std::string::size_type pos = column_dtype.find('(')+1;

    std::string arr_size = std::string(column_dtype.begin()+pos,
                             column_dtype.begin()+column_dtype.find(')'));

    std::istringstream iss(arr_size);

    int arr_size_i;
    iss >> arr_size_i;

    memcpy(page_buffer, &arr_size_i, sizeof(arr_size_i));
}

void generate_file_header(char *page_buffer, std::istream& repl) {
    bool has_primary_key;
    int primary_key_pos = -1;

    repl >> has_primary_key;

    if (has_primary_key)
        repl >> primary_key_pos;

    memcpy(page_buffer, &has_primary_key, sizeof(has_primary_key));
    page_buffer += sizeof(has_primary_key);

    memcpy(page_buffer, &primary_key_pos, sizeof(primary_key_pos));
    page_buffer += sizeof(primary_key_pos);

    int pos = 1;
    memcpy(page_buffer, &pos, sizeof(pos));
    page_buffer += sizeof(pos);

    memcpy(page_buffer, &pos, sizeof(pos));
}

bool create_table(const std::string& table_name, std::istream& repl) {
    // initialize table
    using std::string;

    std::fstream file;
    if (!open_file(file, table_name+".tab"))
        return false;

    char page_buffer[page_size] = {0};

    generate_file_header(page_buffer, repl);
    char *iter = page_buffer + HEADER_SIZE;

    string column_name, column_dtype;

    // TODO: stop before page overflow
    // TODO: check if rows exceed page size
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

        memcpy(iter, column_name.c_str(), column_name.size()+1);
        iter += column_name.size()+1;

        memcpy(iter, column_dtype.c_str(), 1);
        iter += 1;

        if (column_dtype[0] == 'c') {
            write_char_size(iter, column_dtype);
            iter += sizeof(int);
        }
    }

    file.write(page_buffer, page_size);
    file.close();

    return true;
}

void Table::add_root_node() {
    BTreeNode root = pager.insert_new_node();
    root.is_leaf = true;
    root.parent_pos = -1;
    root.next_leaf = -1;
    root.prev_leaf = -1;
    root.n = 0;

    pager.write_node_data(root);
}

void delete_table(const std::string& table_name) {
    std::string file_name = table_name+".tab";
    QFile(QString::fromStdString(file_name)).remove();
}

Table::Table(const std::string& table_name): pager(table_name)  {
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
    if (pager.has_primary_key) {
        pager.set_t();
        pager.init_btree();
    }
}

using namespace std;

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

    // TODO: update so as to only print selected columns from rows
    // TODO: update so that selected rows are returned to this function page by page
    vector<vector<Value>> selected_rows = pager.select_rows(column_pos, val);

    for (auto& row : selected_rows) {
        for (auto& v : row) {
            os << v << " ";
        }
        os << "\n";
    }

    return os;
}

std::ostream& Table::select_all_rows(std::ostream & os) {
    vector<vector<Value>> selected_rows = pager.select_all_rows();

    for (auto& row : selected_rows) {
        for (size_t i = 0; i < row.size(); i++) {
            auto &v = row[i];
            os << v;
            if (i < row.size()-1) os << ",";
        }
        os << "\n";
    }

    return os;
}

std::istream& Table::update_rows(std::istream& is) {
    // TODO: separate WHERE condition from updated column selection
    std::string searched_name;
    is >> searched_name;

    int searched_pos = get_column_position(searched_name, pager.column_names);

    if (searched_pos == -1) {
        clog << "Could not find requested column\n";
        return is;
    }

    Value src_val(pager.column_types[searched_pos], pager.column_sizes[searched_pos]);
    is >> src_val;

    std::string updated_name;
    is >> updated_name;

    int updated_pos = get_column_position(updated_name, pager.column_names);

    if (updated_pos == -1) {
        clog << "Could not find requested column\n";
        return is;
    }

    Value new_val(pager.column_types[updated_pos], pager.column_sizes[updated_pos]);
    is >> new_val;

    pager.update_rows(searched_pos, src_val, updated_pos, new_val);

    return is;
}

std::istream& Table::delete_rows(std::istream& is) {
    std::string column_name;
    is >> column_name;

    int column_pos = get_column_position(column_name, pager.column_names);

    if (column_pos == -1) {
        clog << "Could not find requested column\n";
        return is;
    }

    Value val(pager.column_types[column_pos], pager.column_sizes[column_pos]);
    is >> val;

    pager.delete_rows(column_pos, val);

    return is;
}


QVector<QString> Table::get_column_names() {
    QVector<QString> ret;

    for (auto & column_name : pager.column_names)
        ret.push_back(QString::fromStdString(column_name));

    return ret;
}


QVector<QString> Table::get_column_types() {
    QVector<QString> ret;
    for (auto & column_type : pager.column_types)
        ret.push_back(QString::fromStdString(column_type));

    return ret;
}


QVector<QString> Table::get_column_sizes() {
    QVector<QString> ret;
    for (auto & column_sizes : pager.column_sizes)
        ret.push_back(QString::number(column_sizes));

    return ret;
}


int Table::get_primary_key_pos() {
    return pager.primary_key_pos;
}


int Table::get_column_count() {
    return pager.column_names.size();
}
