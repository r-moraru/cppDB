cmake_minimum_required(VERSION 3.19)
project(cppDB)

set(CMAKE_CXX_STANDARD 20)

add_executable(cppDB main.cpp Ptr/Ptr.h Value/Value.cpp Value/Value.h Table/Table.cpp Table/Table.h
        Row/Row.cpp Row/Row.h Pager/Pager.cpp Pager/Pager.h constants.h Value/String.h Value/Int.h Value/Float.h Value/Base.h BTree/BTree.cpp BTree/BTree.h BTree/BTreeNode.cpp BTree/BTreeNode.h)