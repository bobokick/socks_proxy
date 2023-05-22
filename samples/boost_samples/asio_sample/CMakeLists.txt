message(STATUS "\tentering the dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# 设置可执行文件名
set(EXE_NAME test_asio)

# 所有源文件的路径
set(ALL_SRCS
	main.cc
)

# 寻找boost头文件，库文件等路径
# 使用boost静态库
set(Boost_USE_STATIC_LIBS ON)
# boost库静态链接runtime库
set(Boost_USE_STATIC_RUNTIME OFF)
# 要使用的boost编译的库版本
set(Boost_COMPILER "vc142")
set(BOOST_ROOT ${TP_ROOT_PATH}/boost/build/${CMAKE_BUILD_TYPE})
# set(BOOST_INCLUDEDIR ${BOOST_ROOT}/include)
# 必须添加BOOST_LIBRARYDIR，否则not found
#set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
# set(Boost_NO_SYSTEM_PATHS ON)
# set(Boost_THREADAPI "win32")
# set(Boost_ARCHITECTURE "-x64")
# set(Boost_DEBUG OFF)
find_package(Boost 1.76.0 REQUIRED
   COMPONENTS system filesystem thread 
   )
if(Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
   message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
   link_directories(${Boost_LIBRARY_DIRS})
   message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
   message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
endif()

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
