#ifndef BOOST_ASIO_SAMPLE_NET_TCP_SERVER_ASYNCHRONOUS_H_
#define BOOST_ASIO_SAMPLE_NET_TCP_SERVER_ASYNCHRONOUS_H_

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

using boost::asio::ip::tcp;

// 生成daytime协议数据
std::string makeDaytimeString();

// 已连接套接字的处理类
class TcpConnection
{
    tcp::socket socket_;
    // 私有构造，创建实例包含的套接字
    TcpConnection(boost::asio::io_context& io_context):
        socket_(io_context) {}
    // 关于客户端连接的其他操作将由该函数负责
    // 如错误，长度信息输出等
    // 如不需要处理这些相关信息，则可以将该函数形参表设为空
    // void handleWrite() {}
    void handleWrite(const std::error_code& /*error*/, size_t /*bytes_transferred*/) {}
public:
    // 用共享智能指针是为了保持TcpConnection对象的存活。
    typedef std::shared_ptr<TcpConnection> TcpPointer;
    // 用于控制该类实例的创建，用智能指针控制
    inline static TcpPointer create(boost::asio::io_context& io_context)
    {
        return TcpPointer(new TcpConnection(io_context));
    }
    // 返回该实例包含的套接字
    tcp::socket& getSocket() { return socket_; }
    // 处理client请求（也就是发送数据到client）
    void start()
    {
        // 声明为静态，防止boost::asio::async_write在返回后需要写入时，该变量内存已被释放导致的内存引用错误的问题。
        static std::string message = "";
        // 客户端ip port信息
        std::string client_info = socket_.remote_endpoint().address().to_string() + ":" + std::to_string(socket_.remote_endpoint().port());
        // 输出已连接信息
        std::cout << "connected successfully with client " << client_info << std::endl;
        // 储存将要发送的数据
        message = makeDaytimeString();
        // 发送服务到客户端
        // 注意用的是boost::asio::async_write而不是ip::tcp::socket::async_write_some()，是为了保证整个数据块都能被完整发送。
        // boost::asio::buffer用于将字符串转换成可传输的格式，如二进制，大端格式等。
        // @note: boost::asio::buffer形参为变量的引用，注意内存释放问题。
        // boost::asio::async_write(socket_, boost::asio::buffer(message), std::bind(&TcpConnection::handleWrite, this));
        boost::asio::async_write(socket_, boost::asio::buffer(message), std::bind(&TcpConnection::handleWrite, this, std::placeholders::_1, std::placeholders::_2));
        std::cout << "message has sent to client " << client_info << "\n";
        if (socket_.is_open())
        {
            socket_.shutdown(tcp::socket::shutdown_both);
            socket_.close();
            std::cout << "close the connection\n\n";
        }
    }
};

// tcp异步连接请求接收类
class TcpServer
{
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    int listen_port_;
    // 当startAccept()函数中的异步接收操作完成时，该函数会被调用
    // 它服务客户端请求时，然后会继续调用startAccept进行下一个接收操作。
    void handleAccept(TcpConnection::TcpPointer new_connection, const std::error_code& error)
    {
        // std::cout << "this thread id in handleAccept: " << std::this_thread::get_id() << std::endl;
        // 客户端ip port信息
        std::string client_info = new_connection->getSocket().remote_endpoint().address().to_string() + ":" + std::to_string(new_connection->getSocket().remote_endpoint().port());
        // 输出已连接信息
        // std::cout << "client info: " << client_info << std::endl;
        // 无错误时就开始处理请求
        if (!error)
        {
            // std::cout << "no error" << std::endl;
            new_connection->start();
        }
        else
        {
            std::cout << "error occurred: " << error.message() << std::endl;
        }
        // 继续等待client连接请求
        startAccept();
    }
    // 创建一个套接字并初始化一个异步接收操作用于等待新连接
    void startAccept()
    {
        // std::cout << "this thread id in startAccept: " << std::this_thread::get_id() << std::endl;
        TcpConnection::TcpPointer new_connection = TcpConnection::create(io_context_);
        // 输出等待连接请求信息
        std::cout << "wait for connecting in port " << listen_port_ << std::endl;
        // 绑定套接字（将服务的套接字地址与该套接字连接起来），并进行监听（将套接字转换为监听套接字），。
        // 异步操作，此时不进行阻塞来等待client的连接请求。当调用该函数时，进行线程分发操作，当前线程直接返回，被分发的线程来进行阻塞等待并在连接后调用指定的函数TcpServer::handleAccept。
        acceptor_.async_accept(new_connection->getSocket(), std::bind(&TcpServer::handleAccept, this, new_connection, std::placeholders::_1));
        // std::cout << "connection has built!" << std::endl;
    }
public:
    // 初始化监听对象来监听tcp ipv4上给定端口的连接请求
    TcpServer(boost::asio::io_context& io_context, int listen_port): io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), listen_port)), listen_port_(listen_port)
    {
        startAccept();
    }
};

#endif // !BOOST_ASIO_SAMPLE_NET_TCP_SERVER_ASYNCHRONOUS_H_
