
# 设置需要编译的子文件夹
set(COMPILE_FOLDERS
    gen1
    gen2
    main
)


# 设置boost安装路径
set(BOOST_ROOT ${TP_ROOT_PATH}/boost_pure)
# 设置boost头文件存放路径
set(BOOST_INCLUDEDIR ${TP_ROOT_PATH}/boost_pure)
# 设置boost库文件存放路径
# 必须添加，否则not found
set(BOOST_LIBRARYDIR ${TP_ROOT_PATH}/boost_pure/build/${CMAKE_BUILD_TYPE}/lib)
# 设置是否使用boost静态库
set(Boost_USE_STATIC_LIBS ON)
# 设置是否使用多线程库
set(Boost_USE_MULTITHREADED ON)
# 设置是否不在非指定的相关路径（默认的一些路径）中搜索头库文件
set(Boost_NO_SYSTEM_PATHS ON)
# 设置是否不搜索boost安装文件夹中的config.cmake来指定相关搜索路径
set(Boost_NO_BOOST_CMAKE ON)
# 设置boost库是否静态链接c++ runtime库
set(Boost_USE_STATIC_RUNTIME OFF)
# 设置boost库文件编译时所使用的编译器
set(Boost_COMPILER "-vc142")
# 设置boost库文件编译时所使用的cpu架构
# set(Boost_ARCHITECTURE "-x64")
# 设置是否输出Findboost执行时的输出，用于debug
set(Boost_DEBUG ON)
# 设置是否关闭cmake版本无法识别boost新版本的警告提示
set(Boost_NO_WARN_NEW_VERSIONS ON)

# 寻找boost头文件，库文件等路径
find_package(Boost 1.76.0 REQUIRED
   COMPONENTS system filesystem
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
