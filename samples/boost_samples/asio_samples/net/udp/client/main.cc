#include "client.h"

// udp客户端
void udpClient(std::string server_host)
{
    using boost::asio::ip::udp;
    try
    {
        // 使用asio的都需要至少一个IO执行上下文，如io_context或者thread_pool对象。
        boost::asio::io_context io_context;
        // 用于解析服务器以及协议的
        udp::resolver resolver(io_context);
        // 对服务器以及所需协议进行解析，用daytime协议
        // 返回udp端点，用于进行udp连接。
        udp::endpoint receiver_endpoint = *(resolver.resolve(udp::v4(), server_host, "daytime").begin());
        // 创建套接字
        udp::socket socket(io_context);
        // 发送数据前先指定网络层协议，否则无法进行发送。
        socket.open(udp::v4());
        // 发送数据包到指定的udp端点
        // udp由于是面向数据包类型，不是向tcp一样是流类型的套接字。所以不需要先进行握手，直接发送。
        // 该函数将会阻塞至成功发送数据或者出错。
        socket.send_to(boost::asio::buffer("empty content."), receiver_endpoint);
        // @log for debug
        std::string local_skt_info = socket.local_endpoint().address().to_string() + ":" + std::to_string(socket.local_endpoint().port());
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