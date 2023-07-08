#include "server.h"

// 生成daytime协议数据
std::string makeDaytimeString()
{
    using namespace std;
    time_t now = time(nullptr);
    return ctime(&now);
}

// udp同步服务器，支持ipv4，v6
void udpServer(int listen_port, std::string (*p_makeString) ())
{
    using namespace boost::asio;
    using boost::asio::ip::udp;
    try
    {
        // 使用asio的都需要至少一个IO执行上下文，如io_context或者thread_pool对象。
        boost::asio::io_context io_context;
        // 创建udp套接字用于接收给定端口的udp连接请求（也就是对所有向该端口发送udp数据的，都视为连接请求）
        udp::socket socket(io_context);
        // 打开套接字并指定协议
        socket.open(udp::v6());
        // 设置该套接字可以同时接收ipv4和v6的连接请求。
        // @warning: 设置该属性时套接字要先开启并指定v6协议，且不能先绑定endpoint，否则会出错。
        socket.set_option(ip::v6_only(false));
        // 绑定endpoint
        socket.bind(udp::endpoint(udp::v6(), listen_port));
        // boost::asio::ip::v6_only option(false);
        // socket.get_option(option);
        // bool v6_only = option.value();
        // std::cout << "v6_only: " << v6_only << std::endl;
        // @log for debug
        std::string local_skt_info = (socket.local_endpoint().address().is_v4() ? socket.local_endpoint().address().to_v4().to_string(): socket.local_endpoint().address().to_v6().to_string())+ "/" + std::to_string(socket.local_endpoint().port());
        boost::asio::detail::socket_type local_native_skt = socket.native_handle();
        std::cout << "server has a socket '" << local_skt_info << "/fd:" << local_native_skt << "' for sending data to client.\n";
        // 循环等待client的连接请求并发送数据
        while(true)
        {
            // 用于接收连接请求数据（也就是对所有向该端口发送udp数据的，都视为连接请求）
            std::vector<char> buff(128);
            // 输出等待连接请求信息
            std::cout << "wait for connection request in port " << listen_port << std::endl;
            // 用于储存后续连接的client的udp端口相关信息。
            udp::endpoint remote_endpoint;
            // 接收client的连接请求数据，并将其写入到buff。
            // 该函数将会阻塞至成功接收数据或者出错。
            socket.receive_from(boost::asio::buffer(buff), remote_endpoint);
            // 客户端ip port信息
            std::string client_info = (remote_endpoint.address().is_v4() ? remote_endpoint.address().to_v4().to_string(): remote_endpoint.address().to_v6().to_string())+ "/" + std::to_string(remote_endpoint.port());
            // 输出已连接信息
            std::cout << "received connection request successfully from client " << client_info << std::endl;
            // std::cout << "message:\n" << p_makeString();
            // 储存发送数据操作的错误信息
            boost::system::error_code send_error;
            // 发送数据包到client
            // 该函数将会阻塞至成功发送数据或者出错。
            socket.send_to(boost::asio::buffer(p_makeString()), remote_endpoint, 0, send_error);
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
    udpServer(13, makeDaytimeString);
    return 0;
}