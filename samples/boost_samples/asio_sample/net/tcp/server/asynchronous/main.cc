#include "server.h"

using boost::asio::ip::tcp;

// 生成daytime协议数据
std::string makeDaytimeString()
{
    using namespace std;
    time_t now = time(nullptr);
    return ctime(&now);
}

class TcpConnection
{
    tcp::socket socket_;
    std::string message_;
    TcpConnection(boost::asio::io_context& io_context):
        socket_(io_context) {}
    // 关于客户端连接的其他操作将由该函数负责
    // 如错误，长度信息输出等
    void handleWrite(const boost::system::error_code& /*error*/, size_t /*bytes_transferred*/) {}
public:
    // 用共享智能指针是为了保持TcpConnection对象的存活。
    typedef std::shared_ptr<TcpConnection> CPointer;
    inline static CPointer create(boost::asio::io_context& io_context)
    {
        return CPointer(new TcpConnection(io_context));
    }
    tcp::socket& getSocket() { return socket_; }
    void start()
    {
        // 储存将要发送的数据
        message_ = makeDaytimeString();
        // 发送服务到客户端
        // 注意用的是boost::asio::async_write而不是ip::tcp::socket::async_write_some()，是为了保证整个数据块都能被完整发送。
        boost::asio::async_write(socket_, boost::asio::buffer(message_), std::bind(&TcpConnection::handleWrite, this, std::placeholders::_1, std::placeholders::_2));
    }
};

class TcpServer
{
    boost::asio::io_context& io_context_;
    tcp::acceptor acceptor_;
    // 当startAccept()函数中的异步接收操作完成时，该函数会被调用
    // 它服务客户端请求时，然后会继续调用startAccept进行下一个接收操作。
    void handleAccept(TcpConnection::CPointer new_connection, const boost::system::error_code& error)
    {
        if (!error)
        {
            new_connection->start();
        }
        startAccept();
    }
    // 创建一个套接字并初始化一个异步接收操作用于等待新连接
    void startAccept()
    {
        TcpConnection::CPointer new_connection = TcpConnection::create(io_context_);
        acceptor_.async_accept(new_connection->getSocket(), std::bind(&TcpServer::handleAccept, this, new_connection, std::placeholders::_2));
    }
public:
    // 初始化监听对象来监听tcp ipv4上13号端口的连接请求
    TcpServer(boost::asio::io_context& io_context, int listen_port): io_context_(io_context), acceptor_(io_context, tcp::endpoint(tcp::v4(), listen_port))
    {
        startAccept();
    }
};

// tcp同步服务器
void tcpServer(int listen_port, std::string (*p_makeString) ())
{
    using boost::asio::ip::tcp;
    try
    {
        // 提供I/O服务，用于接收到来的客户端连接，服务器对象将会使用
        boost::asio::io_context io_context;
        // 创建一个监听对象来监听tcp ipv4上13号端口的连接请求
        // 同步的监听对象，一次只能处理一个client的连接请求
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), listen_port));
        // 循环等待client的连接请求并发送数据
        while(true)
        {
            tcp::socket skt(io_context);
            // 输出等待连接请求信息
            std::cout << "wait for connecting in port " << listen_port << std::endl;
            // 绑定套接字，并进行监听，等待client的连接请求。
            acceptor.accept(skt);
            std::string client_info = skt.remote_endpoint().address().to_string() + ":" + std::to_string(skt.remote_endpoint().port());
            // 输出已连接信息
            std::cout << "connected successfully with client " << client_info << std::endl;
            // std::cout << "message:\n" << p_makeString();
            // 将数据写入到套接字中（也就是发送数据给client）
            boost::asio::write(skt, boost::asio::buffer(p_makeString()));
            std::cout << "message has sent to client " << client_info << "\nclose the connection\n\n";
        }
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    try
    {
        // 提供I/O服务，用于接收到来的客户端连接，服务器对象将会使用
        boost::asio::io_context io_context;
        // 进行daytime协议的通信
        TcpServer server(io_context, 13);
        // 运行io对象使之能够执行异步操作
        io_context.run();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
    // tcpServer(13, makeDaytimeString);
}
