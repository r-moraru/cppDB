//
// Created by radua on 10/8/2021.
//

#include "BTree.h"
#include "../Pager/Pager.h"

void BTree::insert(Pager& pager, const Value& key, const std::vector<Value>& data) {
    if (root.n == 2*pager.t-1) {
        BTreeNode s = pager.insert_new_node();

        s.is_leaf = false;
        s.c[0] = root.pos;
        root = s;

        root.split_child(pager, 0, key);
    }

    root.insert_non_full(pager, key, data);
}

void BTree::remove(Pager& pager, const Value& k) {
    root.delete_from_node(pager, k);

    if (!root.is_leaf && root.n == 0) {
        BTreeNode temp = pager.read_node(root.c[0]);
        pager.remove_node(root.pos);

        root = temp;
        pager.set_btree_root(root.pos);
    }
}

void BTree::traverse(Pager& pager) {
    root.traverse(pager);
}

std::vector<Value> BTree::get_data(Pager &pager, const Value &val) {
    return root.get_data(pager, val);
}

BTree::BTree(Pager &pager) : root(pager.read_node(pager.root_pos)) { }
