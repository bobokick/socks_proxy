#ifndef BOOST_ASIO_SAMPLE_NET_UDP_SERVER_ASYNCHRONOUS_H_
#define BOOST_ASIO_SAMPLE_NET_UDP_SERVER_ASYNCHRONOUS_H_

#include <iostream>
#include <string>
#include <ctime>
#include <memory>
#include <functional>
#include <thread>
#include <boost/asio.hpp>
#ifdef _WIN32
#include <sdkddkver.h>
// #define WINVER 0x0A00
// #define _WIN32_WINNT 0x0A00
#endif

using boost::asio::ip::udp;

// 生成daytime协议数据
std::string makeDaytimeString();

// udp异步服务器
class UdpServer
{
    // 用共享智能指针是为了保持TcpConnection对象的存活。
    typedef std::shared_ptr<udp::socket> UdpSktPointer;

    // udp套接字，用于接收ipv4上给定端口的udp连接请求（也就是对所有向该端口发送udp数据的，都视为连接请求）
    UdpSktPointer p_socket_;

    // 用于储存连接的client的udp端口相关信息。
    udp::endpoint remote_endpoint_;

    // client的udp端口的ip port字符串信息。
    std::string client_info_;

    // 协议端口
    int listen_port_;

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
        // 声明为静态，防止async_send_to函数在返回后需要调用给定函数（该例子指定的函数为UdpServer::handleSend）时，
        // 该变量内存已被释放导致的内存引用错误的问题（问题为：由于async_send_to考虑到后续函数操作可能会使用到该message的内容。因此在msvc编译器的debug模式下，调用给定函数前会对该message进行内存检查，失败则抛出运行出错）。
        static std::string message = "";
        // daytime数据
        message = makeDaytimeString();
        // 发送数据包到client
        // 异步操作，此时不进行阻塞来等待发送数据到client完毕。当调用该函数时，进行操作分发，并直接返回。
        // 发送操作执行完毕后会调用指定的函数（该例子指定的函数为UdpServer::handleSend）来进行操作。
        p_socket_->async_send_to(boost::asio::buffer(message), remote_endpoint_, std::bind(&UdpServer::handleSend, this, std::placeholders::_1, std::placeholders::_2));
        if (p_socket_->is_open())
        {
            p_socket_->shutdown(udp::socket::shutdown_both);
            p_socket_->close();
            std::cout << "message has sent to client " << client_info_ << "\n";
            std::cout << "close the connection\n\n";
        }
    }

    //  当startReceive函数中的异步接收操作完成时，该函数会被调用
    // 它服务客户端请求后，然后会继续调用startReceive进行下一个接收操作。
    void handleReceive(const std::error_code& error, const std::size_t& len)
    {
        // 无错误时就开始处理请求
        if (!error)
        {
            // std::cout << "no error" << std::endl;
            // 客户端ip port信息
            client_info_ = remote_endpoint_.address().to_string() + ":" + std::to_string(remote_endpoint_.port());
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
        p_socket_->async_receive_from(boost::asio::buffer(buff_), remote_endpoint_, std::bind(&UdpServer::handleReceive, this, std::placeholders::_1, std::placeholders::_2));
    }

public:
    // 创建udp套接字用于接收ipv4上给定端口的udp连接请求
    UdpServer(boost::asio::io_context& io_context, int listen_port): p_socket_(std::make_shared<udp::socket>(io_context, udp::endpoint(udp::v4(), listen_port))), listen_port_(listen_port), buff_(128)
    {
        // @log for debug
        std::string local_skt_info = p_socket_->local_endpoint().address().to_string() + ":" + std::to_string(p_socket_->local_endpoint().port());
        boost::asio::detail::socket_type local_native_skt = p_socket_->native_handle();
        std::cout << "server has a socket '" << local_skt_info << "/fd:" << local_native_skt << "' for sending data to client.\n";
        startReceive();
    }
};

#endif // !BOOST_ASIO_SAMPLE_NET_TCP_SERVER_ASYNCHRONOUS_H_
