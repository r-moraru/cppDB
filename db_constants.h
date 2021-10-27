//
// Created by radua on 9/30/2021.
//

#ifndef CPPDB_CONSTANTS_H
#define CPPDB_CONSTANTS_H

const std::size_t page_size = 4096;
const std::size_t HEADER_SIZE = sizeof(bool) + 3 *sizeof(int);
const std::size_t BTREENODE_HEADER_SIZE = 5*sizeof(int)+sizeof(bool);
#endif //CPPDB_CONSTANTS_H
