#include "client.h"

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

// udp客户端，支持ipv4，v6
void udpClient(std::string server_host)
{
    using namespace boost::asio;
    using boost::asio::ip::udp;
    try
    {
        // 使用asio的都需要至少一个IO执行上下文，如io_context或者thread_pool对象。
        boost::asio::io_context io_context;
        // 用于解析服务器以及协议的
        udp::resolver resolver(io_context);
        // 对服务器以及所需协议进行解析，用daytime协议
        // 返回udp套接字地址，用于进行udp连接。
        udp::endpoint receiver_endpoint = *(resolver.resolve(server_host, "daytime").begin());
        auto host_addr = ip::address::from_string(server_host);
        // 创建套接字
        udp::socket socket(io_context);
        // 发送数据前先指定网络层协议，否则无法进行发送。
        // 根据给定地址选择v4或v6协议。
        host_addr.is_v4() ? socket.open(udp::v4()): socket.open(udp::v6());
        // 发送数据包到指定的udp套接字地址
        // 由于udp是面向数据包类型，不是向tcp一样是流类型的套接字。所以不需要先进行握手，直接发送。
        // 该函数将会阻塞至成功发送数据或者出错。
        socket.send_to(boost::asio::buffer("empty content.\n"), receiver_endpoint);
        // @log for debug
        std::string local_skt_info = socket.local_endpoint().address().to_string() + "/" + std::to_string(socket.local_endpoint().port());
        // 获取套接字对应的文件描述符
        boost::asio::detail::socket_type local_native_skt = socket.native_handle();
        std::cout << "client has a socket  '" << local_skt_info << "/fd:" << local_native_skt << "' for communicating with server.\n";
        // 输出等待接收daytime协议数据信息。
        std::cout << "ready for receiving daytime data from " << server_host << ":13\n";
        // 用于接收数据
        std::vector<char> buff(128);
        // 用于储存后续接收到udp数据的发送方的udp端口相关信息。
        udp::endpoint sender_endpoint;
        // 储存接收数据操作的错误信息
        boost::system::error_code receive_error;
        // 接收server发送的数据，并将其写入到buff。
        // 返回接收的字节大小。
        // 该函数将会阻塞至成功接收数据或者出错。
        size_t len = socket.receive_from(boost::asio::buffer(buff), sender_endpoint, 0, receive_error);
        // 显示接收的数据
        std::cout << "received message:\n";
        std::cout.write(buff.data(), len);
        std::cout << "close the connection\n";
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
}

int main(int argc, char* argv[])
{
    // 输入server ip
    if (argc != 2)
    {
        std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
        return 1;
    }
    udpClient(argv[1]);
    return 0;
}