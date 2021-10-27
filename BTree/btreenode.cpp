//
// Created by radua on 10/8/2021.
//

#include <QMessageBox>

#include <iostream>
#include <vector>
#include "btreenode.h"
#include "../Pager/pager.h"

using std::vector;

vector<Value> BTreeNode::get_data(Pager& pager, const Value& k) {
    int i = 0;
    while (i < n && k < keys[i])
        i++;

    if (is_leaf) {
        if (keys[i] == k)
            return data[i];
        else
            return vector<Value>();
    }
    else {
        BTreeNode ch = pager.read_node(c[i]);

        return ch.get_data(pager, k);
    }
}

void BTreeNode::split_child(Pager& pager, int idx, const Value& k) {
    BTreeNode y = pager.read_node(c[idx]);
    BTreeNode z = pager.insert_new_node();

    z.parent_pos = pos;
    z.is_leaf = y.is_leaf;

    if (y.is_leaf) {
        z.next_leaf = y.next_leaf;
        if (z.next_leaf != -1) {
            BTreeNode nl = pager.read_node(z.next_leaf);
            nl.prev_leaf = z.pos;
            pager.write_node_data(nl);
        }

        y.next_leaf = z.pos;
        z.prev_leaf = y.pos;
    }

    for (int i = pager.t; i < y.n; i++) {
        z.keys[i-pager.t] = y.keys[i];
        z.data[i-pager.t] = y.data[i];
        z.c[i-pager.t] = y.c[i];
    }

    z.n = pager.t-1;
    z.c[z.n] = y.c[y.n];
    y.n = pager.t;

    Value copy_key = y.keys[y.n-1];
    if (!y.is_leaf) y.n--;

    for (int i = 2*pager.t-2; i >= pager.t; i--) {
        y.c[i+1] = y.c[i+2];
        y.keys[i] = y.keys[i+1];
        y.data[i] = y.data[i+1];
    }

    if (!y.is_leaf) {
        y.c[pager.t] = y.c[pager.t+1];
        y.keys[pager.t-1] = y.keys[pager.t];
    }

    int i;
    c[n+1] = c[n];
    for (i = n-1; i >= 0 && i >= idx; i--) {
        keys[i+1] = keys[i];
        c[i+1] = c[i];
    }

    keys[i+1] = copy_key;
    c[idx+1] = z.pos;

    n++;

    pager.write_node_data(*this);
    pager.write_node_data(y);
    pager.write_node_data(z);
}

void BTreeNode::insert_non_full(Pager& pager, const Value& k, const vector<Value>& d) {
    int i = n-1;

    if (is_leaf) {
        // TODO: check if k is already in tree
        // throw an exception in this case


        while (i >= 0 && (k < keys[i] || k == keys[i])) {
            keys[i+1] = keys[i];
            data[i+1] = data[i];
            i--;
        }

        if (i+1 < n && keys[i+1] == k) {
            QMessageBox ms;
            ms.setText("Duplicate primary key detected.");
            ms.exec();

            return;
        }

        keys[i+1] = k;
        data[i+1] = d;

        n++;

        pager.write_node_data(*this);
    }
    else {
        while (i >= 0 && k < keys[i]) {
            i--;
        }
        i++;

        BTreeNode ch = pager.read_node(c[i]);

        if (ch.n == 2*pager.t-1) {
            split_child(pager, i, k);
            if (keys[i] < k) {
                i++;
            }
        }

        ch = pager.read_node(c[i]);
        ch.insert_non_full(pager, k, d);
    }
}

Value BTreeNode::delete_from_node(Pager& pager, const Value& k) {
    int i = 0;
    while (i < n && keys[i] < k) {
        i++;
    }

    if (!is_leaf) {
        BTreeNode ch = pager.read_node(c[i]);
        if (ch.n < pager.t)
            i = fill(pager, i);

        ch = pager.read_node(c[i]);
        Value new_key = ch.delete_from_node(pager, k);

        if (i < n && keys[i] == k) {
            keys[i] = new_key;
            pager.write_node_data(*this);
        }

        return new_key;
    }
    else {
        if (!(keys[i] == k)) {
            // TODO: key not found, throw exception
            return Value();
        }

        for (int j = i; j < n; j++) {
            keys[j] = keys[j+1];
            data[j] = data[j+1];
        }

        n--;

        pager.write_node_data(*this);
        return keys[std::max(0, i-1)];
    }
}

int BTreeNode::fill(Pager& pager, int i) {
    BTreeNode right_neighbor(pager.t);
    BTreeNode left_neighbor(pager.t);
    if (i < n)
        right_neighbor = pager.read_node(c[i+1]);
    if (i > 0)
        left_neighbor = pager.read_node(c[i-1]);

    if (i < n && right_neighbor.n >= pager.t) {
        add_from_right(pager, i);
    }
    else if (i > 0 && left_neighbor.n >= pager.t) {
        add_from_left(pager, i);
    }
    else if (i < n)
        merge(pager, i);
    else {
        merge(pager, i-1);
        return i-1;
    }
    return i;
}

void BTreeNode::merge(Pager& pager, int i) {
    BTreeNode y = pager.read_node(c[i]),
              z = pager.read_node(c[i+1]);

    if (y.is_leaf) {
        for (int j = 0; j < z.n; j++) {
            y.keys[y.n] = z.keys[j];
            y.data[y.n] = z.data[j];
            y.n++;
        }

        y.next_leaf = z.next_leaf;
        if (y.next_leaf != -1) {
            BTreeNode nl = pager.read_node(y.next_leaf);
            nl.prev_leaf = y.pos;
            pager.write_node_data(nl);
        }
    }
    else {
        y.keys[y.n] = keys[i];
        y.n++;
        y.c[y.n] = z.c[0];

        for (int j = 0; j < z.n; j++) {
            y.keys[y.n] = z.keys[j];
            y.c[y.n+1] = z.c[j+1];
            y.n++;
        }
    }

    for (int j = i; j < n; j++) {
        keys[j] = keys[j+1];
        c[j+1] = c[j+2];
    }

    n--;

    pager.remove_node(z.pos);
    pager.write_node_data(*this);
    pager.write_node_data(y);
}

void BTreeNode::add_from_left(Pager& pager, int i) {
    BTreeNode y = pager.read_node(c[i]),
              z = pager.read_node(c[i-1]);

    if (y.is_leaf) {
        for (int j = y.n; j > 0; j--) {
            y.keys[j] = y.keys[j-1];
            y.data[j] = y.data[j-1];
        }

        y.keys[0] = z.keys[z.n-1];
        y.data[0] = z.data[z.n-1];

        y.n++;

        z.keys[z.n-1] = z.keys[z.n];
        z.data[z.n-1] = z.data[z.n];

        z.n--;

        keys[i-1] = z.keys[z.n-1];
    }
    else {
        y.c[y.n+1] = y.c[y.n];

        for (int j = y.n; j > 0; j--) {
            y.keys[j] = y.keys[j-1];
            y.c[j] = y.c[j-1];
        }

        y.keys[0] = keys[i-1];
        y.c[0] = z.c[z.n];

        y.n++;

        keys[i-1] = z.keys[z.n-1];

        z.keys[z.n-1] = z.keys[z.n];
        z.c[z.n] = z.c[z.n+1];

        z.n--;
    }

    pager.write_node_data(*this);
    pager.write_node_data(y);
    pager.write_node_data(z);
}

void BTreeNode::add_from_right(Pager& pager, int i) {
    BTreeNode y = pager.read_node(c[i]), z = pager.read_node(c[i+1]);

    if (y.is_leaf) {
        y.keys[y.n] = z.keys[0];
        y.data[y.n] = z.data[0];

        y.n++;

        keys[i] = z.keys[0];

        for (int j = 0; j < z.n; j++) {
            z.keys[j] = z.keys[j+1];
            z.data[j] = z.data[j+1];
        }

        z.n--;
    }
    else {
        y.keys[y.n] = keys[i];
        y.c[y.n+1] = z.c[0];

        y.n++;
        keys[i] = z.keys[0];

        for (int j = 0; j < z.n; j++) {
            z.keys[j] = z.keys[j+1];
            z.c[j] = z.c[j+1];
        }

        z.c[z.n] = z.c[z.n+1];
        z.n--;
    }

    pager.write_node_data(*this);
    pager.write_node_data(y);
    pager.write_node_data(z);
}

void BTreeNode::traverse(Pager& pager) {
    if (is_leaf) {
        if (next_leaf != -1) {
            BTreeNode next_leaf_node = pager.read_node(next_leaf);
            next_leaf_node.traverse(pager);
        }
    }
    else {
        BTreeNode ch = pager.read_node(c[0]);
        ch.traverse(pager);
    }
}

void BTreeNode::dfs(Pager& pager, int indentation) {
    std::cout << std::string(indentation, '\t') << "Log: new node with " << n << " keys" << std::endl;
    for (int i = 0; i < n; i++) {
        if (!is_leaf) {
            BTreeNode ch = pager.read_node(c[i]);
            ch.dfs(pager, indentation+1);
        }
        std::cout << std::string(indentation, '\t') << "---- " << keys[i] << std::endl;
    }

    if (!is_leaf) {
        BTreeNode ch = pager.read_node(c[n]);
        ch.dfs(pager, indentation+1);
    }
}

std::vector<std::vector<Value>> BTreeNode::select_rows(Pager &pager,
                                                       const Value &value) {
    using std::vector;
    int i = 0;
    while (i < n && keys[i] < value)
        i++;

    if (is_leaf) {
        vector<vector<Value>> ret;
        if (i < n && value == keys[i])
            ret.push_back(data[i]);

        // TODO: otherwise, throw an exception
        return ret;
    }
    else {
        BTreeNode ch = pager.read_node(c[i]);
        return ch.select_rows(pager, value);
    }
}

std::vector<std::vector<Value>> BTreeNode::select_rows(Pager &pager, int key_pos,
                                                       const Value &value) {
    using std::vector;

    if (is_leaf) {
        vector<vector<Value>> ret;
        for (int i = 0; i < n; i++) {
            if (data[i][key_pos] == value)
                ret.push_back(data[i]);
        }

        if (next_leaf != -1) {
            BTreeNode next = pager.read_node(next_leaf);
            vector<vector<Value>> added_rows = next.select_rows(pager,
                                                                key_pos, value);
            ret.insert(ret.end(), added_rows.begin(), added_rows.end());
        }
        return ret;
    }
    else {
        BTreeNode ch = pager.read_node(c[0]);
        return ch.select_rows(pager, key_pos, value);
    }
}
