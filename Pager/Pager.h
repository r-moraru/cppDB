//
// Created by radua on 9/29/2021.
//

#ifndef CPPDB_PAGER_H
#define CPPDB_PAGER_H

#include <vector>
#include <fstream>
#include <string>
#include "../constants.h"

class Pager {
public:
    explicit Pager(const std::string& file_name);
    // separate data from first page into strings: column_name, column_type, column_size
    std::vector<std::string> get_column_data();
private:
    std::fstream file;
    char page_data[page_size];
    std::fstream::pos_type page_pos;
    std::fstream::pos_type row_pos;
};


#endif //CPPDB_PAGER_H
