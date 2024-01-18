# 构建系统为windows时
if(WIN32)
    # 所有三方库的主路径
    set(TP_ROOT_PATH ${CMAKE_SOURCE_DIR}/third_party)
    # 三方库库文件路径的配置相关
    set(host_os "${CMAKE_SYSTEM_NAME}")
    set(toolset "intel")
    if("${CMAKE_CXX_COMPILER}" MATCHES ".*/g\\+\\+\\.exe")
        set(toolset "mingw")
        # boost asio中会使用
        # 显式连接winsock的库文件，因为#pragma lib对于mingw不生效
        link_libraries(ws2_32 wsock32)
    else()
        # 使用msvc/intel时，禁止boost头文件中使用#pragma lib来连接boost库
        add_compile_definitions(
            BOOST_ALL_NO_LIB
        )
    endif()
    set(library_type "static")
    set(build_type "${CMAKE_BUILD_TYPE}")

    # boost
    set(version_required "v1_76_0")
    set(BOOST_ROOT_DIR ${TP_ROOT_PATH}/boost/${version_required})
    set(BOOST_INCLUDE_DIR ${BOOST_ROOT_DIR}/include)
    set(BOOST_LIB_DIR ${BOOST_ROOT_DIR}/lib/${host_os}/${toolset}/${library_type}/${build_type})
    # message(STATUS "---BOOST_LIB_DIR: ${BOOST_LIB_DIR}---")
    # 导入所有库，使项目在`target_link_libraries`中可以直接写库名(不管前后缀、路径)就能使用。
    # 用于匹配库名称
    if("${toolset}" MATCHES "mingw")
        set(lib_suffix "*.a")
        set(regex_list "^lib" "\\.a")
        set(rep_ex_list "" "")
    else()
        set(lib_suffix "*.lib")
        set(regex_list "^lib" "-.*\\.lib" "\\.lib$")
        set(rep_ex_list "" "" "")
    endif()
    auto_import_libs_in_dir("${BOOST_LIB_DIR}" "${lib_suffix}" regex_list rep_ex_list)

    # googletest
    set(version_required "v1_14_0")
    set(toolset "msvc")
    set(GTEST_ROOT_DIR ${TP_ROOT_PATH}/googletest/${version_required})
    set(GTEST_INCLUDE_DIR ${GTEST_ROOT_DIR}/include)
    set(GTEST_LIB_DIR ${GTEST_ROOT_DIR}/lib/${host_os}/${toolset}/${library_type}/${build_type})
    # message(STATUS "---GTEST_LIB_DIR: ${GTEST_LIB_DIR}---")
    # 导入所有库，使项目在`target_link_libraries`中可以直接写库名(不管前后缀、路径)就能使用。
    # 用于匹配库名称
    if("${toolset}" MATCHES "mingw")
        set(lib_suffix "*.a")
        set(regex_list "^lib" "\\.a")
        set(rep_ex_list "" "")
    else()
        set(lib_suffix "*.lib")
        set(regex_list "^lib" "\\.lib$")
        set(rep_ex_list "" "")
    endif()
    auto_import_libs_in_dir("${GTEST_LIB_DIR}" "${lib_suffix}" regex_list rep_ex_list)
endif()
