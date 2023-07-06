#include "server.h"

// 生成daytime协议数据
std::string makeDaytimeString()
{
    using namespace std;
    time_t now = time(nullptr);
    return ctime(&now);
}

int main(int argc, char* argv[])
{
    try
    {
        // 提供I/O服务，用于接收到来的客户端连接，服务器对象将会使用
        boost::asio::io_context io_context;
        // 进行daytime协议的通信
        TcpServer server(io_context, 13);
        auto thread_call = [](boost::asio::io_context& io) { io.run(); };
        // 开始一个新的thread用来io_context::run（无什么作用）
        //boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
        //std::thread t(thread_call, std::ref(io_context));
        // 运行io对象使之能够执行异步操作
        // 阻塞，直到当前线程的所有operation完成后或者io停止后才会返回。
        io_context.run();
        //t.join();
    }
    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}
