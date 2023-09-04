# cmake版本要求
cmake_minimum_required(VERSION 3.20.3)

# 项目名称：目标可执行文件名或库的名称
project("socks_proxy")

# --------------配置设置-------------

# 默认为debug
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
# if (NOT CMAKE_CONFIGURATION_TYPES)
#     set(CMAKE_CONFIGURATION_TYPES Debug)
# endif()

# for clangd intellisence
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# for dll export
set(WINDOWS_EXPORT_ALL_SYMBOLS ON)

# 根据操作系统增加编译选项
set(CXX_STANDARD 17)

string(REGEX REPLACE "^.*/" "" MAKE_TOOL "${CMAKE_MAKE_PROGRAM}")
string(REGEX REPLACE "\\.exe" "" MAKE_TOOL "${MAKE_TOOL}")
message(STATUS "MAKE_TOOL: ${MAKE_TOOL}")

# 包含三方库路径
include(cmake/auto_import_libs.cmake)
include(cmake/third_party_paths_config.cmake)

# 开启ctest测试，生成Makefile以及相应测试文件后，就可以运行make test来执行用add_test添加的测试了。
#enable_testing()

# --------------配置检测-------------

# 检测编译器
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
message(STATUS "CMAKE_CONFIGURATION_TYPES: ${CMAKE_CONFIGURATION_TYPES}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ARCHITECTURE_ID: ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
message(STATUS "CMAKE_LINKER: ${CMAKE_LINKER}")
message(STATUS "CMAKE_MAKE_PROGRAM: ${CMAKE_MAKE_PROGRAM}")

# --------------构建项目设置-------------

# gtest编译示例测试
add_subdirectory(samples)

message(STATUS "\tentering the dir: ${CMAKE_CURRENT_SOURCE_DIR}")
