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
        // 创建一个监听对象来监听tcp ipv4上给定端口的连接请求
        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), listen_port));
        // 循环等待client的连接请求并发送数据
        while(true)
        {
            // 创建套接字
            tcp::socket skt(io_context);
            // 输出等待连接请求信息
            std::cout << "wait for connecting in port " << listen_port << std::endl;
            // 绑定套接字（将服务的套接字地址与该套接字连接起来），并进行监听（将套接字转换为监听套接字），并进行阻塞用于等待client的连接请求。
            // 同步操作，当前线程一次只能处理一个client的连接请求
            acceptor.accept(skt);
            std::cout << "connection has built!" << std::endl;
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