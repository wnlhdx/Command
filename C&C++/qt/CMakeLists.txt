cmake_minimum_required(VERSION 3.29)

project(qtgame)
set(OpenGL_GL_PREFERENCE GLVND)
set(Vulkan_INCLUDE_DIR "/data/data/com.termux/files/usr/include/vulkan")
find_library(VULKAN_LIBRARY NAMES vulkan libvulkan.so.1 libvulkan.so PATHS /data/data/com.termux/files/usr/lib)
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
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core  Qt6::Widgets Vulkan::Vulkan)


