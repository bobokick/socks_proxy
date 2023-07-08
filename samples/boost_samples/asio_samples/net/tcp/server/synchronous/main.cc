#include "server.h"

// 生成daytime协议数据
std::string makeDaytimeString()
{
    using namespace std;
    time_t now = time(nullptr);
    return ctime(&now);
}

// tcp同步服务器，支持ipv4，v6
void tcpServer(int listen_port, std::string (*p_makeString) ())
{
    using namespace boost::asio;
    using boost::asio::ip::tcp;
    try
    {
        // 提供I/O服务，用于接收到来的客户端连接，服务器对象将会使用
        boost::asio::io_context io_context;
        // 创建套接字，来接受client在给定端口的连接请求。
        tcp::acceptor acceptor(io_context, tcp::v6());
        // 设置该套接字可以同时接收ipv4和v6的连接请求。
        // 调用set_option后，默认就不是监听套接字了。
        // @warning: 设置该属性时套接字要先开启并指定v6协议，且不能先绑定endpoint，否则会出错。
        acceptor.set_option(ip::v6_only(false));
        // 绑定endpoint
        acceptor.bind(tcp::endpoint(tcp::v6(), listen_port));
        // @log for debug
        std::string listen_skt_info = acceptor.local_endpoint().address().to_string() + "/" + std::to_string(acceptor.local_endpoint().port());
        boost::asio::detail::socket_type native_skt = acceptor.native_handle();
        std::cout << "server has a listen socket '" << listen_skt_info << "/fd:" << native_skt << "' for listening connection.\n";
        bool first = true;
        // 循环等待client的连接请求并发送数据
        while(true)
        {
            // 创建一个空的TCP套接字类对象，用于存储后续accept函数返回的已连接套接字。
            tcp::socket skt(io_context);
            // 输出等待连接请求信息
            std::cout << "wait for connecting in port " << listen_port << std::endl;
            // @warning: 因为acceptor设置了属性，默认下为主动套接字，要调用该函数变为监听套接字。
            acceptor.listen();
            // @warning: 只有监听套接字才能正常调用该操作，否则出错。
            // 等待客户端的连接请求，并在收到连接请求后，新建套接字与客户端套接字之间建立连接，并将新建的套接字存储到给定的TCP套接字类对象中。
            // 同步操作，进行阻塞用于等待client的连接请求。
            acceptor.accept(skt);
            // @log log for debug
            if (first)
            {
                std::string local_skt_info = skt.local_endpoint().address().to_string() + "/" + std::to_string(skt.local_endpoint().port());
                boost::asio::detail::socket_type local_native_skt = skt.native_handle();
                std::cout << "server has create a socket '" << local_skt_info << "/fd:" << local_native_skt << "' for connectting client.\n";
                first = false;
            }
            // 客户端ip port信息
            std::string client_info = skt.remote_endpoint().address().to_string() + "/" + std::to_string(skt.remote_endpoint().port());
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