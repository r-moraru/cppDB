//
// Created by radua on 10/8/2021.
//

#ifndef CPPDB_BTREE_H
#define CPPDB_BTREE_H

#include <string>
// #include "../Pager/Pager.h"
#include "btreenode.h"

class Pager;

class BTree {
    friend class Pager;
private:
    BTreeNode root;
private:
    void dfs(Pager& pager) { root.dfs(pager, 0); };
    void insert(Pager&, const Value&, const std::vector<Value>&);

    void remove(Pager&, const Value&);
    void remove(Pager&, int, const Value&);

    std::vector<std::vector<Value>> select_rows(Pager&, const Value&);
    std::vector<std::vector<Value>> select_rows(Pager&, int, const Value&);

    void update_rows(Pager&, const Value&, int, const Value&);
    void update_rows(Pager&, int, const Value&, int, const Value&);
public:
    BTree() : root(0) { }
    // BTree(Pager&);
};


#endif //CPPDB_BTREE_H
