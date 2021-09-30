//
// Created by radua on 9/29/2021.
//

#include <cstring>
#include <string>
#include <vector>
#include "Pager.h"

Pager::Pager(const std::string& file_name): file(file_name), page_data{0} {
    file.read(page_data, page_size);

    page_pos = file.tellg();
    row_pos = file.tellg()+(std::streampos)sizeof(int);
}

std::vector<std::string> Pager::get_column_data() {
    std::vector<std::string> column_data;

    char* iter1 = page_data;
    char* iter2 = strchr(page_data, '\0');

    while (iter2 != nullptr && iter2-page_data <= page_size) {
        if (*iter1 == 0)
            break;

        std::string column_name = std::string(iter1, iter2);
        column_data.emplace_back(iter1, iter2);

        // move iterator to first char after column name
        iter1 = iter2+1;

        // read column type
        std::string column_type = std::string(iter1, iter1+1);

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

        // get next column name
        iter2 = strchr(iter1, '\0');
    }

    return column_data;
}