# CMakeList.txt: demo2 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.29)


set(CMAKE_C_STANDARD 23) # 设置 C 语言标准


project ("theory")


# 定义源文件
set(SOURCES   "theory.c") # 其他源文件

# 将源代码添加到此项目的可执行文件。
add_executable (theory "theory.c")


# TODO: 如有需要，请添加测试并安装目标。
