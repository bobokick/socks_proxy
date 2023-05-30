#include "client.h"

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
        // 返回一个tcp端点，该端点用于进行tcp连接，支持迭代器。
        tcp::resolver::results_type endpoints = resolver.resolve(server_host, "daytime");
        // 创建套接字
        tcp::socket socket(io_context);
        // 进行套接字连接，自动检测ip版本如ipv4, ipv6进行连接。
        boost::asio::connect(socket, endpoints);
        while(true)
        {
            // 用于接收数据
            std::vector<char> buff;
            boost::system::error_code error;
            // 接收server发送的数据，并将其写入到buff
            // 返回接收的字节大小
            size_t len = socket.read_some(boost::asio::buffer(buff), error);
            // 全部接收完毕时，关闭连接
            if (error == boost::asio::error::eof)
                break; // Connection closed cleanly by peer.
            else if (error)
                throw boost::system::system_error(error); // Some other error.
            // 显示接收的数据
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
    if (argc != 2)
    {
        std::cerr << "Usage: client <host>" << std::endl;
        return 1;
    }
    tcpClient(argv[1]);
    return 0;
}