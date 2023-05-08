
cmake_minimum_required(VERSION 3.8)

project(boostdll_test1)

set(CMAKE_CXX_STANDARD 11)

set(compile_type SHARED)

add_library(${PROJECT_NAME} ${compile_type} gen.cc)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${TP_ROOT_PATH}/boost
)
target_link_directories(${PROJECT_NAME} PRIVATE 

)
target_link_libraries(${PROJECT_NAME} 
    Boost::filesystem
)

# 使用生成器表达式以及target_properties指定target生成的位置，
# 避免msbuild等生成器会自动加上Debug/Realease等后缀
# 指定静态库文件输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_SOURCE_DIR}/lib>)
# 指定动态库文件输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_SOURCE_DIR}/lib>)
# 指定可执行程序输出目录
# windows上dll生成位置使用的是RUNTIME_OUTPUT_DIRECTORY变量
if (CMAKE_SYSTEM_NAME MATCHES "Windows" AND ${compile_type} MATCHES "SHARED")
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_SOURCE_DIR}/lib>)
else()
	set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_CURRENT_SOURCE_DIR}/bin>)
endif()
