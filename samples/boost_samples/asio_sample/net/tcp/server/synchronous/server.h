#include <iostream>
#include <string>
#include <ctime>
#include <boost/asio.hpp>
#ifdef _WIN32
#include <sdkddkver.h>
// #define WINVER 0x0A00
// #define _WIN32_WINNT 0x0A00
#endif

#ifndef BOOST_ASIO_SAMPLE_NET_TCP_SERVER_SYNCHRONOUS_H
#define BOOST_ASIO_SAMPLE_NET_TCP_SERVER_SYNCHRONOUS_H

// 生成daytime协议数据
std::string makeDaytimeString();

// tcp同步服务器
void tcpServer(int listen_port, std::string (*p_makeString) ());

#endif // !BOOST_ASIO_SAMPLE_NET_TCP_SERVER_SYNCHRONOUS_H
