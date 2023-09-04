#ifndef BOOST_NO_LIB_H_
#define BOOST_NO_LIB_H_

#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include <boost/lexical_cast.hpp>

double add(const std::string& int_num, const std::string& dou_num)
{
    return boost::lexical_cast<int>(int_num) + boost::lexical_cast<double>(dou_num);
}

int main()
{
    std::cout << "add(\"1225\", \"1548.15\"):" << std::endl;
    std::cout << add("1225", "1548.15") << std::endl;
    return 0;
}

#endif
