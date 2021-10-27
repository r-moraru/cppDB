//
// Created by radua on 10/8/2021.
//

#include <QMessageBox>

#include "btree.h"
#include "../Pager/pager.h"

#include <iostream>

void BTree::insert(Pager& pager, const Value& key, const std::vector<Value>& data) {
    if (root.n == 2*pager.t-1) {
        BTreeNode s = pager.insert_new_node();
        pager.set_btree_root(s.pos);

        s.is_leaf = false;
        s.c[0] = root.pos;
        root = s;

        root.split_child(pager, 0, key);
    }

    root.insert_non_full(pager, key, data);
}

std::vector<std::vector<Value>> BTree::select_rows(Pager &pager, const Value &key) {
    return root.select_rows(pager, key);
}

std::vector<std::vector<Value>> BTree::select_rows(Pager &pager, int key_pos,
                                                   const Value &key) {
    return root.select_rows(pager, key_pos, key);
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

void BTree::remove(Pager& pager, int key_pos, const Value& k) {
    std::vector<std::vector<Value>> selected_rows = select_rows(pager, key_pos, k);

    for (auto& row : selected_rows)
        remove(pager, row[pager.primary_key_pos]);
}

void BTree::update_rows(Pager& pager, const Value &key,
                        int updated_pos, const Value &new_val) {
    QMessageBox ms;
    ms.setText("b tree update with primary search key");
    ms.exec();
    std::vector<std::vector<Value>> selected_rows = select_rows(pager, key);
    for (auto& row : selected_rows) {
        remove(pager, row[pager.primary_key_pos]);
        row[updated_pos] = new_val;
        insert(pager, row[pager.primary_key_pos], row);
    }
}

void BTree::update_rows(Pager &pager, int key_pos,
                        const Value &key,
                        int updated_pos, const Value &new_val) {
    QMessageBox ms;
    ms.setText("b tree update with non-primary search key");
    ms.exec();
    std::vector<std::vector<Value>> selected_rows = select_rows(pager, key_pos, key);
    for (auto& row : selected_rows) {
        remove(pager, row[pager.primary_key_pos]);
        row[updated_pos] = new_val;
        QMessageBox ms;
        ms.setText("new_value: ");
        ms.exec();
        insert(pager, row[pager.primary_key_pos], row);
    }
}
