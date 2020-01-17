cmake_minimum_required(VERSION 3.13)
project(Belish)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        include
)

add_executable(Belish main.cpp src/Belish.cpp include/Belish.h include/lex.h src/lex.cpp include/dylib.h src/dylib.cpp src/ast.cpp include/ast.h src/compiler.cpp include/compiler.h src/trans.cpp include/trans.h src/vm.cpp include/vm.h src/values.cpp include/values.h)