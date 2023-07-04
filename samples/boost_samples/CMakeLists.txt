
# 设置需要编译的子文件夹
set(COMPILE_FOLDERS
    asio_samples
    dll_load
    no_lib
)

#####################################################################以下不需要进行手动设置###########################################################################

# 对于msvc编译器，取消#pragma lib自动链接库
if (${CMAKE_CXX_COMPILER} MATCHES ".*cl.exe")
    add_compile_definitions(
        BOOST_ALL_NO_LIB
    )
endif()

# 需要编译的子项目。
foreach(folder ${COMPILE_FOLDERS})
    add_subdirectory("${folder}")
endforeach()
