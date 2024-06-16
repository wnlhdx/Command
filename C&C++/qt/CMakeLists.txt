# CMakeList.txt: demo2 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.29)

project ("mygame")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)



if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # 64-bit architecture, assume it's x86 Windows with Vulkan SDK installed
  set(Qt6_DIR  "D:\\software\\study\\develop\\Qt6\\6.7.1\\msvc2019_64\\lib\\cmake")
  set(CMAKE_PREFIX_PATH "D:\\software\\study\\develop\\Qt6\\6.7.1\\msvc2019_64"})
else()
  # 32-bit architecture, assume it's ARM Termux
  set(CMAKE_PREFIX_PATH "/data/data/com.termux/files/usr/lib/qt6")
endif()



find_package(Qt6  REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

qt_add_executable(${PROJECT_NAME}
    mygame.cpp
)

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
)
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set_target_properties(${PROJECT_NAME}  PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
    )
endif()




 # 将源代码添加到此项目的可执行文件。










