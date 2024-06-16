# CMakeList.txt: demo2 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # 64-bit architecture, assume it's x86 Windows with Vulkan SDK installed
  set(Qt6_DIR "D:\software\study\develop\qt\cmake")
else()
  # 32-bit architecture, assume it's ARM Termux
  set(Qt6_DIR "/data/data/com.termux/files/usr/include/qt6/")
endif()

 find_package(Qt6 COMPONENTS Core Quick Multimedia Svg Charts REQUIRED)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


project ("qt")

# 将源代码添加到此项目的可执行文件。
add_executable (qt "qt.cpp")

target_link_libraries(qt Qt6::Core Qt6::Quick Qt6::Multimedia Qt6::Svg Qt6::Charts)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET qt PROPERTY CXX_STANDARD 23)
endif()

# TODO: 如有需要，请添加测试并安装目标。
