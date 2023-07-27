cmake_minimum_required(VERSION 3.16)

project(Compiler)
set(CMAKE_CXX_STANDARD 14)

# 查找 LLVM 库
find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")

# 设置包含目录
include_directories(${LLVM_INCLUDE_DIRS})

# 添加 LLVM 定义
add_definitions(${LLVM_DEFINITIONS})

# 添加可执行文件
add_executable(
    Compiler
    src/lex.yy.cpp
    src/parse.cpp
    src/parse.hpp
    src/node.h
    src/context.cpp
    src/context.h
    src/genJSON.cpp
    src/genCode.cpp
    src/util.cpp
    src/main.cpp
)

set(LLVM_LINK_COMPONENTS
    Core
    ExecutionEngine
    Interpreter
    MC
    Support
    nativecodegen
    mcjit
    asmparser
    x86asmparser
)
llvm_map_components_to_libnames(llvm_libs ${LLVM_LINK_COMPONENTS})

# 链接 LLVM 库
target_link_libraries(Compiler ${llvm_libs})
