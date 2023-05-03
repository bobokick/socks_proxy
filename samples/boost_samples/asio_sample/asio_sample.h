#include <iostream>
#include <string>
#include <boost/asio.hpp>

#ifndef BOOST_ASIO_SAMPLE_H
#define BOOST_ASIO_SAMPLE_H

int main()
{
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, boost::asio::chrono::seconds(5));
    t.wait();
    std::cout << "Hello, world!" << std::endl;
    return 0;
}

#endif // !BOOST_ASIO_SAMPLE_H
