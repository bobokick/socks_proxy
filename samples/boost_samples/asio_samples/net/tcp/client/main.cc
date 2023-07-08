#include "client.h"

// tcp客户端，支持ipv4，v6
void tcpClient(std::string server_host)
{
    using boost::asio::ip::tcp;
    try
    {
        // 使用asio的都需要至少一个IO执行上下文，如io_context或者thread_pool对象。
        boost::asio::io_context io_context;
        // 用于解析服务器以及协议的
        tcp::resolver resolver(io_context);
        // 对服务器以及所需协议进行解析，用daytime协议
        // 返回包含至少一个tcp端点的解析结果迭代器（只要不解析失败），该结果迭代器用于获取解析后的tcp端点用于进行tcp连接。
        tcp::resolver::results_type endpoint_iter = resolver.resolve(server_host, "daytime");
        // 输出连接请求相关信息
        std::cout << "ready for connecting " << server_host << "/13 using by daytime protocol\n";
        // 创建套接字
        // @warning: 同一网络适配器，同种ip协议下的所有进程的套接字都会关联唯一的端口和该端口对应的唯一io描述符。因此在同一网络适配器，同种ip协议下不能有多个套接字使用相同的端口。
        tcp::socket socket(io_context);
        // 请求套接字连接
        // 可以自动检测ip版本如ipv4, ipv6进行连接。
        // 自动打开套接字并进行连接请求，如果套接字之前已经打开，则自动关闭后再进行。
        // 该函数会尝试对迭代器中的每一个tcp端点进行连接，直到成功连接到一个。
        boost::asio::connect(socket, endpoint_iter);
        // @log for debug
        std::string local_skt_info = socket.local_endpoint().address().to_string() + "/" + std::to_string(socket.local_endpoint().port());
        boost::asio::detail::socket_type local_native_skt = socket.native_handle();
        std::cout << "client has create a socket '" << local_skt_info << "/fd:" << local_native_skt << "' for connectting server.\n";
        // 输出已连接信息
        std::cout << "connected successfully to the server " << server_host << "/13\n";
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
        std::cerr << "Usage: " << argv[0] << " <host>" << std::endl;
        return 1;
    }
    tcpClient(argv[1]);
    return 0;
}