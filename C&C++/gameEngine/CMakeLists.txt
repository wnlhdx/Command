cmake_minimum_required(VERSION 3.10)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


project ("gameEngine")

# Check if we are building for x86 Windows or ARM Termux
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
  # 64-bit architecture, assume it's x86 Windows with Vulkan SDK installed
  find_package(Vulkan REQUIRED)
else()
  # 32-bit architecture, assume it's ARM Termux
  set(Vulkan_INCLUDE_DIRS "/data/data/com.termux/files/usr/include/vulkan/")
  set(Vulkan_LIBRARIES "/data/data/com.termux/files/usr/lib/libvulkan.so")
endif()

# 将源代码添加到此项目的可执行文件。
add_executable (gameEngine "gameEngine.cpp" "graphicsAPI.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET gameEngine PROPERTY CXX_STANDARD 20)
endif()

# Include Vulkan headers
include_directories(${Vulkan_INCLUDE_DIRS})

# Link Vulkan library
target_link_libraries(gameEngine ${Vulkan_LIBRARIES})


# TODO: 如有需要，请添加测试并安装目标。
