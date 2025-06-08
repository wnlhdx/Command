cmake_minimum_required(VERSION 3.8)
project(compiler C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)

# 指定 bison 目标
BISON_TARGET(Parser parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.c
             DEFINES_FILE ${CMAKE_CURRENT_BINARY_DIR}/parser.tab.h)

# 指定 flex 目标
FLEX_TARGET(Lexer lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lexer.yy.c)

# 解决 bison/flex 依赖关系
ADD_FLEX_BISON_DEPENDENCY(Lexer Parser)

# 构建最终编译器程序
add_executable(compiler
    ${BISON_Parser_OUTPUTS}
    ${FLEX_Lexer_OUTPUTS}
)

# 让 flex 能 include parser.tab.h
target_include_directories(compiler PRIVATE ${CMAKE_CURRENT_BINARY_DIR})