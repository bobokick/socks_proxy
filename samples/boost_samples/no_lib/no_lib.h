#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include <boost/lexical_cast.hpp>
#include <boost/lambda/lambda.hpp>

#ifndef BOOST_NO_LIB_H
#define BOOST_NO_LIB_H

double add(const std::string& intNum, const std::string& douNum)
{
    return boost::lexical_cast<int>(intNum) + boost::lexical_cast<double>(douNum);
}

int main()
{
    std::cout << "add(\"1225\", \"1548.15\"):" << std::endl;
    std::cout << add("1225", "1548.15") << std::endl;
    return 0;
}

#endif // !BOOST_NO_LIB_H
