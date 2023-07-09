#include "server.h"

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
        // @warning: 设置该属性时套接字要先开启并指定v6协议，且不能先绑定套接字地址，否则会出错。
        socket.set_option(ip::v6_only(false));
        // 绑定套接字地址
        // @warning: bind时套接字要先开启，否则会出错。
        socket.bind(udp::endpoint(udp::v6(), listen_port));
        // boost::asio::ip::v6_only option(false);
        // socket.get_option(option);
        // bool v6_only = option.value();
        // std::cout << "v6_only: " << v6_only << std::endl;
        // @log for debug
        std::string local_skt_info = (socket.local_endpoint().address().is_v4() ? socket.local_endpoint().address().to_v4().to_string(): socket.local_endpoint().address().to_v6().to_string())+ "/" + std::to_string(socket.local_endpoint().port());
        // 获取套接字对应的文件描述符
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