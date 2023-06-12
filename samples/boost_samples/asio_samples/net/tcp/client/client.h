#ifndef BOOST_ASIO_SAMPLE_NET_TCP_CLINET_H_
#define BOOST_ASIO_SAMPLE_NET_TCP_CLINET_H_

#include <iostream>
#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#ifdef _WIN32
#include <sdkddkver.h>
// #define WINVER 0x0A00
// #define _WIN32_WINNT 0x0A00
#endif

void tcpClient(std::string server_host);

#endif // !BOOST_ASIO_SAMPLE_NET_TCP_CLINET_H_
