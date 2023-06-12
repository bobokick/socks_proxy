message(STATUS "\tentering the dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 设置可执行文件名
set(EXE_NAME test_asio_net_tcp_server_asynchronous)

# 所有源文件的路径
set(ALL_SRCS
	main.cc
)

# 生成并链接可执行文件
add_executable(${EXE_NAME} ${ALL_SRCS})

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
	# 头文件路径
	target_include_directories(${EXE_NAME} PRIVATE
		${TP_ROOT_PATH}/boost
		)
endif()

# 使用生成器表达式以及target_properties指定target生成的位置，
# 避免msbuild等生成器会自动加上Debug/Realease等后缀
# 指定可执行程序输出目录
set_target_properties(${EXE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${CMAKE_BINARY_DIR}/bin/samples>)
