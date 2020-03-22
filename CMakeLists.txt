cmake_minimum_required(VERSION 3.13)
project(Belish)

set(SHARED OFF)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

include_directories(
        include
)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)
if (APPLE)
    find_library(CF_FRAMEWORK CoreFoundation)
    message("CoreFoundation Framework: ${CF_FRAMEWORK}")
    set(FRAMEWORKS ${CF_FRAMEWORK})
endif()

if (SHARED)
    message("lib")
    add_library(Belish SHARED src/Belish.cpp include/Belish.h include/lex.h src/lex.cpp include/dylib.h src/dylib.cpp src/ast.cpp include/ast.h src/compiler.cpp include/compiler.h src/trans.cpp include/trans.h src/vm.cpp include/vm.h src/values.cpp include/values.h src/decompile.cpp include/decompile.h include/timestamp.h src/timestamp.cpp src/fio.cpp include/fio.h src/valueTracker.cpp include/valueTracker.h src/GC.cpp include/GC.h src/arg.cpp include/arg.h)
else()
    message("exe")
    add_executable(Belish main.cpp src/Belish.cpp include/Belish.h include/lex.h src/lex.cpp include/dylib.h src/dylib.cpp src/ast.cpp include/ast.h src/compiler.cpp include/compiler.h src/trans.cpp include/trans.h src/vm.cpp include/vm.h src/values.cpp include/values.h src/decompile.cpp include/decompile.h include/timestamp.h src/timestamp.cpp src/fio.cpp include/fio.h src/valueTracker.cpp include/valueTracker.h src/GC.cpp include/GC.h src/arg.cpp include/arg.h)
endif()