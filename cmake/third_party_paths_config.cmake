if(WIN32)
    set(TP_ROOT_PATH ${CMAKE_SOURCE_DIR}/third_party)

    # boost
    set(BOOST_ROOT_DIR ${TP_ROOT_PATH}/boost)
    set(BOOST_INCLUDE_DIR ${BOOST_ROOT_DIR}/include)
    set(BOOST_LIB_DIR ${BOOST_ROOT_DIR}/lib_${CMAKE_BUILD_TYPE})
    # 在windows上禁用#pragma lib自动链接boost库
    add_compile_definitions(
        BOOST_ALL_NO_LIB
    )
    # 导入所有boost库，使项目在`target_link_libraries`中可以直接写库名(不管前后缀、路径)就能使用。
    set(regex_list "^lib" "-.*\.lib")
    set(rep_ex_list "" "")
    auto_import_libs_in_dir("${BOOST_LIB_DIR}" regex_list rep_ex_list)
endif()
