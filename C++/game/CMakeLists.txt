﻿# CMakeList.txt: demo1 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.29)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(CMAKE_CXX_STANDARD 23)

project ("game")

# 将源代码添加到此项目的可执行文件。
add_executable (game   "game.cpp" )

if (CMAKE_VERSION VERSION_GREATER 3.29)
  set_property(TARGET game PROPERTY CXX_STANDARD 23)
endif()

# TODO: 如有需要，请添加测试并安装目标。
