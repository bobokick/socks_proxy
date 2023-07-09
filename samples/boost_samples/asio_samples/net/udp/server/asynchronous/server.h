#ifndef BOOST_ASIO_SAMPLE_NET_UDP_SERVER_ASYNCHRONOUS_H_
#define BOOST_ASIO_SAMPLE_NET_UDP_SERVER_ASYNCHRONOUS_H_

#include <iostream>
#include <string>
#include <ctime>
#include <memory>
#include <functional>
#include <vector>
#include <thread>
#include <boost/asio.hpp>
#ifdef _WIN32
#include <sdkddkver.h>
// #define WINVER 0x0A00
// #define _WIN32_WINNT 0x0A00
#endif

/**
 * 
 * @note: 
 * 套接字通常意义指的就是网络连接（连接为点对点，全双工）的一个端点。
 * 毎个套接字都有相应的套接字地址，是由一个因特网地址和一个16位的整数端口的元组对组成的，用(地址, 端口)来表示。
 * 一个连接是由它两端的套接字地址唯一确定的。这对套接字地址叫做套接字对（socket pair), 由下列元组来表示：
 * (cliaddr: cliport, servaddr: servport)
 * @note:
 * 套接字在程序代码的角度可以看作为包含一个给定套接字地址的文件描述符对象，用于进行一些IO操作。
 * 由于同程序中文件描述符是唯一的，因此多个套接字不能含有同一个文件描述符。
 * 多个套接字包含的套接字地址可以是一样的，但该操作只存在于监听套接字分发已连接套接字来进行并发通讯的情况。
 * 不能主动将两个套接字的套接字地址设置成一样，因为设置为一样，就违反了连接点对点的性质，系统就无法确定该和谁通信。
 */

using boost::asio::ip::udp;

// 生成daytime协议数据
std::string makeDaytimeString();

// udp异步服务器
class UdpServer
{
    // udp套接字，用于接收ipv4上给定端口的udp连接请求（也就是对所有向该端口发送udp数据的，都视为连接请求）
    udp::socket socket_;

    // 用于储存连接的client的udp端口相关信息。
    udp::endpoint remote_endpoint_;

    // client的udp端口的ip port字符串信息。
    std::string client_info_;

    // 协议端口
    int listen_port_;

    // 发送数据的生成
    std::string (*p_data_gen_) ();

    // 用于接收连接请求数据
    std::vector<char> buff_;

    // 处理异步发送操作执行后返回的相关信息
    // 如错误，长度信息输出等
    // 如不需要处理这些相关信息，则可以将该函数形参表设为空
    void handleSend(const std::error_code& error, const std::size_t& len)
    {

    }

    // 发送数据
    void sendData()
    {
        // @warning: 声明为静态，防止async_send_to函数在返回后需要调用给定函数（该例子指定的函数为UdpServer::handleSend）时，
        // 该变量内存已被释放导致的内存引用错误的问题（问题为：由于async_send_to考虑到后续函数操作可能会使用到该message的内容。因此在msvc编译器的debug模式下，调用给定函数前会对该message进行内存检查，失败则抛出运行出错）。
        static std::string message = "";
        // daytime数据
        message = p_data_gen_();
        // 发送数据包到client
        // 异步操作，此时不进行阻塞来等待发送数据到client完毕。当调用该函数时，进行操作分发，并直接返回。
        // 发送操作执行完毕后会调用指定的函数（该例子指定的函数为UdpServer::handleSend）来进行操作。
        socket_.async_send_to(boost::asio::buffer(message), remote_endpoint_, std::bind(&UdpServer::handleSend, this, std::placeholders::_1, std::placeholders::_2));
        // @warning: 不要关闭udp套接字。
        // 由于udp是面向数据包类型，不是向tcp一样是流类型的套接字。
        // 没有握手分手阶段，所以不需要关闭udp套接字，可以对多个client使用同一个udp套接字。
        if (socket_.is_open())
        {
            // socket_.shutdown(udp::socket::shutdown_both);
            // socket_.close();
            std::cout << "message has sent to client " << client_info_ << "\n";
            std::cout << "close the connection\n\n";
        }
    }

    // 当startReceive函数中的异步接收操作完成时，该函数会被调用
    // 它服务客户端请求后，然后会继续调用startReceive进行下一个接收操作。
    void handleReceive(const std::error_code& error, const std::size_t& len)
    {
        // 无错误时就开始处理请求
        if (!error)
        {
            // std::cout << "no error" << std::endl;
            // 客户端ip port信息
            client_info_ = remote_endpoint_.address().to_string() + "/" + std::to_string(remote_endpoint_.port());
            // 输出已连接信息
            std::cout << "received connection request successfully from client " << client_info_ << std::endl;
            // 进行数据发送
            sendData();
        }
        else
        {
            std::cout << "error occurred: " << error.message() << std::endl;
        }
        // 继续等待client连接请求
        startReceive();
    }

    // 异步接收client连接请求（也就是对所有向该端口发送udp数据的，都视为连接请求）
    void startReceive()
    {
        // 输出等待连接请求信息
        std::cout << "wait for connection request in port " << listen_port_ << std::endl;
        // 接收client的连接请求数据，并将其写入到buff。
        // 异步操作，此时不进行阻塞来等待client的连接请求。当调用该函数时，进行操作分发，并直接返回。
        // 后续在调用io.run()处进行阻塞等待，并在该套接字收到连接请求后进行请求数据接收，并后续调用指定的函数（该例子指定的函数为UdpServer::handleReceive）来进行操作。
        socket_.async_receive_from(boost::asio::buffer(buff_), remote_endpoint_, std::bind(&UdpServer::handleReceive, this, std::placeholders::_1, std::placeholders::_2));
    }

public:
    // 创建udp套接字用于接收ipv4上给定端口的udp连接请求
    UdpServer(boost::asio::io_context& io_context, int listen_port, std::string (*p_data_gen) () = makeDaytimeString): socket_(io_context, udp::endpoint(udp::v4(), listen_port)), listen_port_(listen_port), p_data_gen_(p_data_gen), buff_(128)
    {
        // @log for debug
        std::string local_skt_info = socket_.local_endpoint().address().to_string() + "/" + std::to_string(socket_.local_endpoint().port());
        // 获取套接字对应的文件描述符
        boost::asio::detail::socket_type local_native_skt = socket_.native_handle();
        std::cout << "server has a socket '" << local_skt_info << "/fd:" << local_native_skt << "' for sending data to client.\n";
        startReceive();
    }
};

#endif // !BOOST_ASIO_SAMPLE_NET_TCP_SERVER_ASYNCHRONOUS_H_
