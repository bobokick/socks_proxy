
# 设置需要编译的子文件夹
set(COMPILE_FOLDERS
    basic_sample
    net
)

# 寻找boost头文件，库文件等路径
# 使用boost静态库
set(Boost_USE_STATIC_LIBS ON)
# boost库静态链接runtime库
set(Boost_USE_STATIC_RUNTIME OFF)
# 要使用的boost编译的库版本
set(Boost_COMPILER "vc142")
set(BOOST_ROOT ${TP_ROOT_PATH}/boost/build/${CMAKE_BUILD_TYPE})
# 必须添加BOOST_LIBRARYDIR，否则not found
# set(BOOST_INCLUDEDIR ${TP_ROOT_PATH}/boost/boost)
# set(BOOST_LIBRARYDIR ${BOOST_ROOT}/lib)
# set(Boost_NO_SYSTEM_PATHS ON)
# set(Boost_THREADAPI "win32")
# set(Boost_ARCHITECTURE "-x64")
# set(Boost_DEBUG OFF)
find_package(Boost REQUIRED
   COMPONENTS system filesystem thread 
   )
if(Boost_FOUND)
   include_directories(${Boost_INCLUDE_DIRS})
   message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
   link_directories(${Boost_LIBRARY_DIRS})
   message(STATUS "Boost_LIBRARY_DIRS: ${Boost_LIBRARY_DIRS}")
   message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")
endif()

#####################################################################以下不需要进行手动设置###########################################################################

# 需要编译的子项目。
foreach(folder ${COMPILE_FOLDERS})
    add_subdirectory("${folder}")
endforeach()
