#include <iostream>
#include <string>
#include <cstdio>
#include <boost/dll.hpp>

#ifndef BOOST_GEN_LIB2
#define BOOST_GEN_LIB2

// NOLINTBEGIN

// 只需在任意声明或定义时写上该宏就行
#define DLL_EXPORT extern "C" BOOST_SYMBOL_EXPORT

DLL_EXPORT void showNumber(int);
DLL_EXPORT void showInformation(const std::string& str)
{
    std::cout << str << std::endl;
}

void showNumber(int ins)
{
    printf("%d\n", ins);
}

void showFixedInfo()
{
    std::cout << "showFixedInfo\n";
}

DLL_EXPORT std::string show_string = "for test";

DLL_EXPORT constexpr int num_about_cpp_standard_version = 17;

// 写函数或变量的别名
BOOST_DLL_ALIAS(num_about_cpp_standard_version, cpp_ver_alias)

// NOLINTEND

#endif // !BOOST_GEN_LIB2