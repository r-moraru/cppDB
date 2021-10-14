//
// Created by radua on 10/8/2021.
//

#ifndef CPPDB_BTREENODE_H
#define CPPDB_BTREENODE_H

#include <vector>
#include "../Value/Value.h"
// #include "../Pager/Pager.h"

class Pager;

class BTreeNode {
    friend class Table;
    friend class Pager;
    friend class BTree;
private:
    int pos;
    int parent_pos;
    std::vector<Value> keys;
    std::vector<std::vector<Value>> data;
    std::vector<int> c;
    int next_leaf;
    int prev_leaf;
    bool is_leaf;
    int n;

private:
    explicit BTreeNode(int t) : next_leaf(-1),
                  prev_leaf(-1), is_leaf(false),
                  pos(-1), parent_pos(-1), n(0),
                  keys(2*t), data(2*t),
                  c(2*t+1) { }

    void traverse(Pager&);
    void dfs(Pager&, int);

    std::vector<Value> get_data(Pager&, const Value&);

    std::vector<std::vector<Value>> select_rows(Pager&, const Value&);
    std::vector<std::vector<Value>> select_rows(Pager&, int, const Value&);

    void split_child(Pager&, int, const Value&);
    void insert_non_full(Pager&, const Value&, const std::vector<Value>&);

    Value delete_from_node(Pager&, const Value&);
    int fill(Pager&, int);

    void merge(Pager&, int);
    void add_from_right(Pager&, int);
    void add_from_left(Pager&, int);
};


#endif //CPPDB_BTREENODE_H
