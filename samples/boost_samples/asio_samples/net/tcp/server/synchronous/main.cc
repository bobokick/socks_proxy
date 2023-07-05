#include "server.h"

// 生成daytime协议数据
std::string makeDaytimeString()
{
    using namespace std;
    time_t now = time(nullptr);
    return ctime(&now);
}

// tcp同步服务器
void tcpServer(int listen_port, std::string (*p_makeString) ())
{
    using boost::asio::ip::tcp;
    try
    {
        boost::asio::io_context io_context;
        // 初始化监听对象（也就是建立监听套接字）来监听tcp ipv4上给定端口的连接请求。
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), listen_port));
        // @log for debug
        // std::string listen_skt_info = acceptor.local_endpoint().address().to_string() + ":" + std::to_string(acceptor.local_endpoint().port());
        // boost::asio::detail::socket_type native_skt = acceptor.native_handle();
        // std::cout << "server has a listen socket '" << listen_skt_info << "', fd: " << native_skt << " for listening connection.\n";
        bool first = true;
        // 循环等待client的连接请求并发送数据
        while(true)
        {
            // 创建一个空的TCP套接字类对象，用于存储后续accept函数返回的已连接套接字。
            tcp::socket skt(io_context);
            // 输出等待连接请求信息
            std::cout << "wait for connecting in port " << listen_port << std::endl;
            // 等待客户端的连接请求，并在收到连接请求后，新建套接字与客户端套接字之间建立连接，并将新建的套接字存储到给定的TCP套接字类对象中。
            // 同步操作，进行阻塞用于等待client的连接请求。
            acceptor.accept(skt);
            std::cout << "connection has built!" << std::endl;
            // @log log for debug
            // if (first)
            // {
            //     std::string local_skt_info = skt.local_endpoint().address().to_string() + ":" + std::to_string(skt.local_endpoint().port());
            //     boost::asio::detail::socket_type local_native_skt = skt.native_handle();
            //     std::cout << "server has create a new socket  '" << local_skt_info << "', fd: " << local_native_skt << " for connectting client.\n";
            //     first = false;
            // }
            // 客户端ip port信息
            std::string client_info = skt.remote_endpoint().address().to_string() + ":" + std::to_string(skt.remote_endpoint().port());
            // 输出已连接信息
            std::cout << "connected successfully with client " << client_info << std::endl;
            // std::cout << "message:\n" << p_makeString();
            // 将数据写入到套接字中（也就是发送数据给client）
            // boost::asio::buffer用于将字符串转换成可传输的格式，如二进制，大端格式等。
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
    // 进行daytime协议的通信
    tcpServer(13, makeDaytimeString);
    return 0;
}