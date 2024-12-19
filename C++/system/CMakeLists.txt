# CMakeList.txt: demo2 的 CMake 项目，在此处包括源代码并定义
# 项目特定的逻辑。
#
cmake_minimum_required (VERSION 3.8)

project ("mySystem")

# 设置 C++ 标准版本
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 查找 C++ 源文件
aux_source_directory(. SRC_CPP_FILES)

# 根据平台选择不同的汇编文件
if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(ASM_FILE "./testwin.asm")
elseif(CMAKE_SYSTEM_NAME MATCHES "Linux")
    set(ASM_FILE "./testlinux.asm")
endif()

message(STATUS "SRC_CPP_FILES: ${SRC_CPP_FILES}")
message(STATUS "ASM_FILE: ${ASM_FILE}")


# 设置汇编文件为 C 语言，以便可以使用 C 编译器选项
set_source_files_properties(${ASM_FILE} PROPERTIES LANGUAGE C)

# 添加可执行文件
add_executable(mySystem  ${ASM_FILE} ${SRC_CPP_FILES})


# 针对不同的平台设置编译选项
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    # 设置适用于 Termux on ARM 的编译选项
    set(CMAKE_SYSTEM_PROCESSOR arm)
    set(CMAKE_C_COMPILER /path/to/arm-linux-gnueabi-gcc)
    set(CMAKE_CXX_COMPILER /path/to/arm-linux-gnueabi-g++)
    set(CMAKE_ASM_COMPILER /path/to/arm-linux-gnueabi-as)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    # 设置适用于 x64 Windows 的编译选项
    set(CMAKE_SYSTEM_PROCESSOR AMD64)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /arch:AVX2")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX2")
endif()

# 包含目录
include_directories(include)

# 链接库
# target_link_libraries(MyExecutable some_library)
