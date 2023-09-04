#ifndef BOOST_ASIO_SAMPLE_NET_UDP_SERVER_SYNCHRONOUS_H_
#define BOOST_ASIO_SAMPLE_NET_UDP_SERVER_SYNCHRONOUS_H_

#include <iostream>
#include <string>
#include <ctime>
#include <boost/asio.hpp>
#ifdef _WIN32
#include <sdkddkver.h>
// #define WINVER 0x0A00
// #define _WIN32_WINNT 0x0A00
#endif

// 生成daytime协议数据
std::string makeDaytimeString();

// udp同步服务器
void udpServer(int listen_port, std::string (*p_makeString) ());

#endif // !BOOST_ASIO_SAMPLE_NET_TCP_SERVER_SYNCHRONOUS_H_
