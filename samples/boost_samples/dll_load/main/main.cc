#include <iostream>
#include <string>
#include <boost/dll.hpp>
#include "lib_path_autogen.h"

using namespace boost;

std::string g_lib_path = LIB1_PATH;
std::string g_lib2_path = LIB2_PATH;

/**
 * @brief 方式1，先加载动态库
 * 
 */
void sample1()
{
    boost::dll::shared_library lib;

    // 加载动态库
    // load选择的路径可以是相对路径，不过如果是相对路径，则是相对于运行可执行文件的目录的相对路径。
    lib.load(g_lib_path);
    // 导入函数符号
    auto show_info = lib.get<void(const std::string&)>("showInformation");
    std::cout << "show_info(\"strings\"):\t";
    show_info("strings");
    std::cout << std::endl;
    // 导入变量符号
    auto cpp_version = lib.get<int>("num_about_cpp_standard_version");
    std::cout << "cpp_version:\t";
    std::cout << cpp_version << std::endl;
// mingw对于boost库'boost/dll.hpp'中的BOOST_DLL_ALIAS语法有bug，无法正常导出该别名导致其别名无法使用。
#if !defined(SYSTEM_IS_Windows) || defined(CPP_COMPILER_IS_MSVC)
    // 导入符号的别名
    auto cpp_version2 = lib.get_alias<int>("cpp_ver");
    std::cout << "cpp_ver:\t";
    std::cout << cpp_version2 << std::endl;
#endif

    // 更改库的加载
    lib.load(g_lib2_path);
    // 导入函数符号
    auto show_num = lib.get<void(int)>("showNumber");
    std::cout << "show_num(-48):\t";
    show_num(-48);
    std::cout << std::endl;
    // 导入变量符号
    auto string_flag = lib.get<std::string>("show_string");
    std::cout << "string_flag:\t";
    std::cout << string_flag << std::endl;
#if !defined(SYSTEM_IS_Windows) || defined(CPP_COMPILER_IS_MSVC)
    // 导入符号的别名
    auto cpp_version3 = lib.get_alias<int>("cpp_ver_alias");
    std::cout << "cpp_ver_alias:\t";
    std::cout << cpp_version3 << std::endl;
#endif
}

/**
 * @brief 方式2，使用时加载动态库
 * 
 */
void sample2()
{
    // 导入库1的函数符号
    auto str = dll::import_symbol<std::string>(g_lib_path, "show_string");
    std::cout << "show_string:\t";
    std::cout << *str.get() << std::endl;
    // 导入库2的函数符号
    auto show_info = dll::import_symbol<void()>(g_lib2_path, "showFixedInfo");
    std::cout << "show_info():\t";
    show_info();
    std::cout << std::endl;
    // 导入变量符号
    auto string_flag = dll::import_symbol<std::string>(g_lib_path, "show_string");
    std::cout << "string_flag:\t";
    std::cout << *string_flag.get() << std::endl;
#if !defined(SYSTEM_IS_Windows) || defined(CPP_COMPILER_IS_MSVC)
    // 导入符号的别名
    auto cpp_version = dll::import_alias<int>(g_lib2_path, "cpp_ver_alias");
    std::cout << "cpp_version:\t";
    std::cout << *cpp_version.get() << std::endl;
#endif
    // 导入非c函数符号（先用nm查函数重整名）
    //auto show_info2 = dll::import_symbol<void ()>(g_lib_path, "_Z13showFixedInfov");
    //std::cout << "show_info2():\n";
    //show_info2();
}

int main()
{
    sample1();
    sample2();
#ifdef SYSTEM_IS_Windows
    SET_DLL_FIND_DIR(BOOST_LIB_PATH);
#endif
    return 0;
}
