
# 导入${need_imported_libs_dir}目录中的所有后缀为${lib_suffix}的项，使项目在`target_link_libraries`中可以直接写库名(不管前后缀、路径)就能使用。
# need_imported_libs_dir: 需要进行导入的库的目录，里面的库所有后缀为${lib_suffix}的项都会被导入
# lib_suffix: 需要导入的所有库的后缀。
# regex_list: 需要进行处理的库名称的正则表达式列表。
# replace_expression_list: 替换regex匹配到的表达式的字符串的列表。列表中的每个元素会与regex_list的对应位置的元素进行匹配替换。
macro(auto_import_libs_in_dir need_imported_libs_dir lib_suffix regex_list replace_expression_list)
    # @debug
    # message(STATUS "---INIT need_imported_libs_dir: ${need_imported_libs_dir}")
    # message(STATUS "---INIT lib_suffix: ${lib_suffix}")
    # message(STATUS "---INIT regex_list: ${regex_list}")
    # message(STATUS "---INIT replace_expression_list: ${replace_expression_list}")
    # 获取给定目录中的所有lib
    file(GLOB ALL_ORIGIN_LIBS 
        LIST_DIRECTORIES false
        RELATIVE ${need_imported_libs_dir}
        CONFIGURE_DEPENDS
        ${need_imported_libs_dir}/${lib_suffix}
    )
    # 每个库进行字符串处理并导入
    foreach(lib_original_name ${ALL_ORIGIN_LIBS})
        # 获取库的基本名称(无前后缀、路径)。
        set(lib_name "${lib_original_name}")
        # 根据给定匹配列表逐个进行字符串处理。
        set(index "0")
        list(LENGTH regex_list list_len)
        while(${index} LESS ${list_len})
            list(GET regex_list ${index} regex)
            # @debug
            # message(STATUS "---ADDRESS regex: ${regex}")
            list(GET replace_expression_list ${index} replace_expression)
            # @debug
            # message(STATUS "---ADDRESS replace_expression: ${replace_expression}")
            if(NOT replace_expression)
                string(REGEX REPLACE ${regex} "" lib_name ${lib_name})
            else()
                string(REGEX REPLACE ${regex} ${replace_expression} lib_name ${lib_name})
            endif()
            # @debug
            # message(STATUS "---ADDRESS lib_name: ${lib_name}")
            math(EXPR index "${index}+1" )
        endwhile()
        # 导入库
        add_library(${lib_name} STATIC IMPORTED)
        set_target_properties(${lib_name}
            PROPERTIES IMPORTED_LOCATION ${need_imported_libs_dir}/${lib_original_name}
        )
        # @debug
        message(STATUS "-----IMPORTED LIBRARY: '${lib_name}'\tTHE IMPORTED_LOCATION: '${need_imported_libs_dir}/${lib_original_name}'-----")
    endforeach()
endmacro()
