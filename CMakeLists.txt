cmake_minimum_required(VERSION 3.19)
project(cppDB)

set(CMAKE_CXX_STANDARD 14)

add_executable(cppDB main.cpp Ptr.cpp Ptr.h Value.cpp Value.h Table/Table.cpp Table/Table.h Row/Row.cpp Row/Row.h Pager/Pager.cpp Pager/Pager.h)