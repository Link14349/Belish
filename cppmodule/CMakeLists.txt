cmake_minimum_required(VERSION 3.14)
project(cppmodule)

set(CMAKE_CXX_STANDARD 17)

include_directories(../include)
LINK_DIRECTORIES(../)

add_library(cppmodule SHARED library.cpp library.h)