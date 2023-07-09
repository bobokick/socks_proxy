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
        // 对指定的协议以及地址（指定的远程套接字地址）进行解析
        // 返回包含至少一个tcp套接字地址的解析结果迭代器（只要不解析失败），该结果迭代器用于获取解析后的tcp套接字地址用于进行tcp连接。
        tcp::resolver::results_type endpoint_iter = resolver.resolve(server_host, "daytime");
        // 输出连接请求相关信息
        std::cout << "ready for connecting " << server_host << "/13 using by daytime protocol\n";
        // 创建套接字
        tcp::socket socket(io_context);
        // 请求套接字连接
        // 可以自动检测ip版本如ipv4, ipv6进行连接。
        // 自动打开套接字并进行连接请求，如果套接字之前已经打开，则自动关闭后再进行。
        // 该函数会尝试对迭代器中的每一个tcp套接字地址进行连接，直到成功连接到一个。
        boost::asio::connect(socket, endpoint_iter);
        // @log for debug
        std::string local_skt_info = socket.local_endpoint().address().to_string() + "/" + std::to_string(socket.local_endpoint().port());
        // 获取套接字对应的文件描述符
        boost::asio::detail::socket_type local_native_skt = socket.native_handle();
        std::cout << "client has a socket '" << local_skt_info << "/fd:" << local_native_skt << "' for connectting server.\n";
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