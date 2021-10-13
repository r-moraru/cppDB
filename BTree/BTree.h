//
// Created by radua on 10/8/2021.
//

#ifndef CPPDB_BTREE_H
#define CPPDB_BTREE_H

#include <string>
// #include "../Pager/Pager.h"
#include "BTreeNode.h"

class Pager;

class BTree {
    friend class Pager;
private:
    BTreeNode root;
private:
    std::vector<Value> get_data(Pager&, const Value&);
    void insert(Pager&, const Value&, const std::vector<Value>&);
    void remove(Pager&, const Value&);
    void traverse(Pager&);

public:
    BTree() : root(0) { }
    BTree(Pager&);
};


#endif //CPPDB_BTREE_H
