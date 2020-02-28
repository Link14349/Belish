cmake_minimum_required(VERSION 3.13)
project(io)

set(CMAKE_CXX_STANDARD 17)
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(../include)
LINK_DIRECTORIES(../)

add_library(io SHARED library.cpp library.h)