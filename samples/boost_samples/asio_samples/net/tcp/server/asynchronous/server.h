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

    // 处理异步写入操作执行后返回的相关信息
    // 如错误，长度信息输出等
    // 如不需要处理这些相关信息，则可以将该函数形参表设为空
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
    void addressRequest(std::string (*p_data_gen) ())
    {
        // @log for debug
        std::string local_skt_info = socket_.local_endpoint().address().to_string() + "/" + std::to_string(socket_.local_endpoint().port());
        // 获取套接字对应的文件描述符
        boost::asio::detail::socket_type local_native_skt = socket_.native_handle();
        std::cout << "server has created a new socket '" << local_skt_info << "/fd:" << local_native_skt << "' for connectting client.\n";
        // @warning: 声明为静态，防止boost::asio::async_write在返回后需要调用给定函数（该例子指定的函数为TcpConnection::handleWrite）时，
        // 该变量内存已被释放导致的内存引用错误的问题（问题为：由于boost::asio::async_write考虑到后续函数操作可能会使用到该message的内容。因此在msvc编译器的debug模式下，调用给定函数前会对该message进行内存检查，失败则抛出运行出错）。
        static std::string message = "";
        // 客户端ip port信息
        std::string client_info = socket_.remote_endpoint().address().to_string() + "/" + std::to_string(socket_.remote_endpoint().port());
        // 输出已连接信息
        std::cout << "connected successfully with client " << client_info << std::endl;
        // 储存将要发送的数据
        message = p_data_gen();
        // 发送服务到客户端
        // 异步操作，此时不进行阻塞来等待发送数据到client完毕。当调用该函数时，进行操作分发，并直接返回。
        // 发送操作执行完毕后会调用指定的函数（该例子指定的函数为TcpConnection::handleWrite）来进行操作。
        // 注意用的是boost::asio::async_write而不是ip::tcp::socket::async_write_some，是为了保证整个数据块都能被完整发送。
        // boost::asio::buffer用于将字符串转换成可传输的格式，如二进制，大端格式等。
        boost::asio::async_write(socket_, boost::asio::buffer(message), std::bind(&TcpConnection::handleWrite, this, std::placeholders::_1, std::placeholders::_2));
        if (socket_.is_open())
        {
            socket_.shutdown(tcp::socket::shutdown_both);
            socket_.close();
            std::cout << "message has sent to client " << client_info << "\n";
            std::cout << "close the connection\n\n";
        }
    }
};

// tcp异步连接请求接收类
class TcpServer
{
    boost::asio::io_context& io_context_;

    tcp::acceptor acceptor_;

    // 发送数据的生成
    std::string (*p_data_gen_) ();

    // 协议端口
    int listen_port_;

    // 当startAccept()函数中的异步接收操作完成时，该函数会被调用
    // 它服务客户端请求后，然后会继续调用startAccept进行下一个接收操作。
    void handleAccept(TcpConnection::TcpPointer new_connection, const std::error_code& error)
    {
        // std::cout << "this thread id in handleAccept: " << std::this_thread::get_id() << std::endl;
        // 无错误时就开始处理请求
        if (!error)
        {
            // std::cout << "no error" << std::endl;
            new_connection->addressRequest(p_data_gen_);
        }
        else
        {
            std::cout << "error occurred: " << error.message() << std::endl;
        }
        // 继续等待client连接请求
        startAccept();
    }

    // 异步接收client连接请求（也就是对所有向该端口发送udp数据的，都视为连接请求）
    void startAccept()
    {
        // std::cout << "this thread id in startAccept: " << std::this_thread::get_id() << std::endl;
        // @log for debug
        std::string listen_skt_info = acceptor_.local_endpoint().address().to_string() + "/" + std::to_string(acceptor_.local_endpoint().port());
        // 获取套接字对应的文件描述符
        boost::asio::detail::socket_type native_skt = acceptor_.native_handle();
        std::cout << "server has a listen socket '" << listen_skt_info << "/fd:" << native_skt << "' for listening connection.\n";
        // 创建一个tcp套接字，用于存储后续async_accept函数返回的已连接套接字。
        TcpConnection::TcpPointer new_connection = TcpConnection::create(io_context_);
        // 输出等待连接请求信息
        std::cout << "wait for connecting in port " << listen_port_ << std::endl;
        // 等待客户端的连接请求，并在收到连接请求后，新建套接字与客户端套接字之间建立连接，并将新建的已连接套接字信息存储到给定的tcp套接字中。
        // 异步操作，此时不进行阻塞来等待client的连接请求。当调用该函数时，进行操作分发，并直接返回。
        // 后续在调用io.run()处进行阻塞等待，并在该套接字收到连接请求后进行tcp连接，并后续调用指定的函数（该例子指定的函数为TcpServer::handleAccept）来进行操作。
        acceptor_.async_accept(new_connection->getSocket(), std::bind(&TcpServer::handleAccept, this, new_connection, std::placeholders::_1));
    }
public:
    // 初始化监听对象（也就是建立监听套接字）来监听tcp ipv4上给定端口的连接请求。
    TcpServer(boost::asio::io_context& io_context, int listen_port, std::string (*p_data_gen) () = makeDaytimeString): io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), listen_port)), listen_port_(listen_port), p_data_gen_(p_data_gen)
    {
        startAccept();
    }
};

#endif // !BOOST_ASIO_SAMPLE_NET_TCP_SERVER_ASYNCHRONOUS_H_
