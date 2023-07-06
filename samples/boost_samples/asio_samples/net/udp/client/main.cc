#include "client.h"

// udp客户端
void udpClient(std::string server_host)
{
    using boost::asio::ip::tcp;
    using boost::asio::ip::udp;
    try
    {
        // 使用asio的都需要至少一个IO执行上下文，如io_context或者thread_pool对象。
        boost::asio::io_context io_context;
        // 用于解析服务器以及协议的
        udp::resolver resolver(io_context);
        // 对服务器以及所需协议进行解析，用daytime协议
        // 获取解析后的tcp端点，该端点用于进行tcp连接。
        udp::endpoint endpoint = *(resolver.resolve(udp::v4(), server_host, "daytime").begin());
        // 输出连接请求相关信息
        std::cout << "ready for connecting " << server_host << ":13 using by daytime protocol\n";
        // 创建套接字
        tcp::socket socket(io_context);
        // 请求套接字连接，自动检测ip版本如ipv4, ipv6进行连接。
        boost::asio::connect(socket, endpoint);
        // @log for debug
        std::string local_skt_info = socket.local_endpoint().address().to_string() + ":" + std::to_string(socket.local_endpoint().port());
        boost::asio::detail::socket_type local_native_skt = socket.native_handle();
        std::cout << "client has create a socket  '" << local_skt_info << "', fd: " << local_native_skt << " for connectting server.\n";
        // 输出已连接信息
        std::cout << "connected successfully to the server " << server_host << ":13\n";
        // 一直接收数据直到到数据尾
        while(true)
        {
            // 用于接收数据
            std::vector<char> buff(128);
            boost::system::error_code error;
            // 接收server发送的数据，并将其写入到buff
            // 返回接收的字节大小
            size_t len = socket.read_some(boost::asio::buffer(buff), error);
            // 全部接收完毕时，关闭连接
            if (error == boost::asio::error::eof)
            {
                std::cout << "close the connection\n";
                break; // Connection closed cleanly by peer.
            }
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            // 显示接收的数据
            std::cout << "received message:\n";
            std::cout.write(buff.data(), len);
        }
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
        std::cerr << "Usage: client <host>" << std::endl;
        return 1;
    }
    udpClient(argv[1]);
    return 0;
}