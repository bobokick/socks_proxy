# cmake版本要求
cmake_minimum_required(VERSION 3.20.3)

# 项目名称：目标可执行文件名或库的名称
project("socks_proxy")

# 检测编译器
message(STATUS "CMAKE_SYSTEM_NAME: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMAKE_C_COMPILER: ${CMAKE_C_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ARCHITECTURE_ID: ${CMAKE_CXX_COMPILER_ARCHITECTURE_ID}")
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_MAKE_PROGRAM: ${CMAKE_MAKE_PROGRAM}")
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")

# 根据操作系统增加编译选项
set(CXX_STANDARD 17)
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
elseif(CMAKE_SYSTEM_NAME MATCHES "Windows")
    set(vs_install_dir $ENV{VS2020PATH})
endif()

# if(MSVC)
#     set(variables 
#         CMAKE_CXX_FLAGS_DEBUG
#         CMAKE_CXX_FLAGS_RELEASE
#         CMAKE_CXX_FLAGS_RELWITHDEBINFO CMAKE_CXX_FLAGS_MINSIZEREL
#     )
#     foreach(variable ${variables})
#         message(STATUS "BEFORE ${variable}: ${${variable}}")
#         if(${variable} MATCHES "/MD")
#             string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
#         endif()
#         message(STATUS "AFTER ${variable}: ${${variable}}")
#     endforeach()
# endif()

set(TP_ROOT_PATH ${CMAKE_SOURCE_DIR}/third_parties)

# 第三方库单独进行cmake及编译
find_package (Python3 COMPONENTS Interpreter)
# 需要编译的第三方库
set(precompile_module_path
    ${TP_ROOT_PATH}/boost
)
foreach(module_path ${precompile_module_path})
    if(IS_DIRECTORY ${CMAKE_SOURCE_DIR}/${module_path}/build)
        message(STATUS "no exist ${CMAKE_SOURCE_DIR}/${module_path}/build")
        execute_process(
            COMMAND ${Python3_EXECUTABLE} precompile.py ${module_path} ${CMAKE_SYSTEM_NAME} ${used_cxx_compiler} ${CMAKE_MAKE_PROGRAM} ${CMAKE_BUILD_TYPE}
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            TIMEOUT 0
            RESULT_VARIABLE result_var
            OUTPUT_VARIABLE output_var
            ERROR_VARIABLE error_var
            COMMAND_ECHO STDOUT
            OUTPUT_STRIP_TRAILING_WHITESPACE
            ERROR_STRIP_TRAILING_WHITESPACE)
        message(STATUS "result: ${result_var}")
        message(STATUS "output: \n${output_var}")
        message(STATUS "error: \n${error_var}")
    endif()
endforeach()

# 开启ctest测试，生成Makefile以及相应测试文件后，就可以运行make test来执行用add_test添加的测试了。
#enable_testing()

# gtest编译示例测试
add_subdirectory(samples)

message(STATUS "\tentering the dir: ${CMAKE_CURRENT_SOURCE_DIR}")