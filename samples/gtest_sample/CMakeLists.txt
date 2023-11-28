message(STATUS "\tentering the dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 设置可执行文件名
set(EXE_NAME test_gtest)

# 所有源文件的路径
set(ALL_SRCS
	main.cc
)

# 生成并链接可执行文件
add_executable(${EXE_NAME} ${ALL_SRCS})

# 头文件所在路径
target_include_directories(${EXE_NAME} PRIVATE
    ${GTEST_INCLUDE_DIR}
)
# 静态库文件所在路径
target_link_directories(${EXE_NAME} PRIVATE
)
# 链接静态库
target_link_libraries(${EXE_NAME} PRIVATE
    gtest
    $<$<PLATFORM_ID:Linux>:pthread>
)

# 使用生成器表达式以及target_properties指定target生成的位置，
# 避免msbuild等生成器会自动加上Debug/Realease等后缀
# 指定可执行程序输出目录
set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin/samples>)