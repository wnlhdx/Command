cmake_minimum_required(VERSION 3.22.1)

project("vulkanTest")

# 创建并命名库
add_library(${CMAKE_PROJECT_NAME} SHARED
    native-lib.cpp)

# 设置 Vulkan 包含目录和库
set(Vulkan_INCLUDE_DIR "/data/data/com.termux/files/usr/include/vulkan")
find_library(VULKAN_LIBRARY NAMES vulkan libvulkan.so.1 libvulkan.so PATHS /data/data/com.termux/files/usr/lib)


# 包含 Vulkan 头文件目录
include_directories(${Vulkan_INCLUDE_DIR})

# 查找并链接 Vulkan 库
target_link_libraries(${CMAKE_PROJECT_NAME}
    ${VULKAN_LIBRARY}
    android
    log)
