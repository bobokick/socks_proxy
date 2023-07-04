
cmake_minimum_required(VERSION 3.8)

project(test_boost_dllload)

set(CMAKE_CXX_STANDARD 11)

# 配置路径代码宏文件生成
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_path_macro.h.in
    
    ${CMAKE_CURRENT_SOURCE_DIR}/lib_path_autogen.h
    @ONLY
)

add_executable(${PROJECT_NAME} main.cc)

target_include_directories(${PROJECT_NAME} PRIVATE 
    ${TP_ROOT_PATH}/boost_pure
)

target_link_directories(${PROJECT_NAME} PRIVATE 

)

target_link_libraries(${PROJECT_NAME} 
    Boost::filesystem
    $<$<PLATFORM_ID:Linux>:dl>
)

add_dependencies(${PROJECT_NAME} 
    boostdll_test_lib1
    boostdll_test_lib2
)

message(STATUS ${Boost_LIBRARIES})

# 使用生成器表达式以及target_properties指定target生成的位置，
# 避免msbuild等生成器会自动加上Debug/Realease等后缀
# 指定可执行程序输出目录
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin/samples>)
