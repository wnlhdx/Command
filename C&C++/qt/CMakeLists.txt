cmake_minimum_required(VERSION 3.29)

project(qtgame)
set(OPENGL_INCLUDE_DIR "/data/data/com.termux/files/usr/include")
set(OPENGL_gl_LIBRARY "/data/data/com.termux/files/usr/lib/libGL.so")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_PREFIX_PATH "/data/data/com.termux/files/usr/lib/cmake")
set(Qt6_DIR "/data/data/com.termux/files/usr/lib/cmake/Qt6/")



# 指定使用 Qt6
find_package(Qt6 COMPONENTS  Widgets  REQUIRED)




set(PROJECT_SOURCES
        main.cpp
        qtgame.cpp
        qtgame.h
        qtgame.ui
)


add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})


# 添加 Vulkan 库
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core  Qt6::Widgets)


