#include <iostream>
#include <string>
#include <chrono>
#include <vector>
#include <boost/asio.hpp>
#include <boost/thread/thread.hpp>
#ifdef _WIN32
#include <sdkddkver.h>
// #define WINVER 0x0A00
// #define _WIN32_WINNT 0x0A00
#endif

#ifndef BOOST_ASIO_SAMPLE_BASIC_H
#define BOOST_ASIO_SAMPLE_BASIC_H

void usingTimerSynchronously()
{
    // 使用asio的都需要至少一个IO执行上下文，如io_context或者thread_pool对象。
    boost::asio::io_context io;
    // 注意：过期时间从计时器建立就开始计时。
    boost::asio::steady_timer t(io, std::chrono::seconds(5));
    std::cout << "start wait for the timer expiration!" << std::endl;
    t.wait();
    std::cout << "waited completed. Hello, usingTimerSynchronously!" << std::endl;
    std::cout << std::endl;
}

void usingTimerAsynchronously()
{
    auto print = [] (const boost::system::error_code& )
    { std::cout << "Hello, usingTimerAsynchronously!" << std::endl; };
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, std::chrono::seconds(5));
    t.async_wait(print);
    std::cout << "start wait for the timer expiration!" << std::endl;
    // 阻塞，直到当前线程的所有work完成后或者io停止后才会返回。
    io.run();
    std::cout << std::endl;
}

void bindingArgsToCompletionHandler()
{
    // 可调用类
    struct Print
    {
        void operator()(const boost::system::error_code&, boost::asio::steady_timer& timer, int& count) const
        {
            if (count < 5)
            {
                std::cout << ++count << std::endl;
                // 设置计时器的总过期时间，过期时间从计时器建立就开始计时，因为需要每次加1s，否则会直接过期。
                timer.expires_at(timer.expiry() + std::chrono::seconds(1));
                timer.async_wait(std::bind(*this, std::placeholders::_1, std::ref(timer), std::ref(count)));
            }
            else
                std::cout << "Hello, bindingArgsToCompletionHandler!" << std::endl;
        }
    };
    int count = 0;
    boost::asio::io_context io;
    boost::asio::steady_timer t(io, std::chrono::seconds(1));
    t.async_wait(std::bind(Print{}, std::placeholders::_1, std::ref(t), std::ref(count)));
    std::cout << "start wait for the timer expiration!" << std::endl;
    io.run();
    std::cout << std::endl;
}

void usingMemberFunctionAsCompletionHandler()
{
    class Printer
    {
        boost::asio::steady_timer timer_;
        int count_;
    public:
        void print()
        {
            if (count_ < 5)
            {
                std::cout << ++count_ << std::endl;
                // 设置计时器的总过期时间，过期时间从计时器建立就开始计时，因为需要每次加1s，否则会直接过期。
                timer_.expires_at(timer_.expiry() + std::chrono::seconds(1));
                timer_.async_wait(std::bind(&Printer::print, this));
            }
        }
        ~Printer() { std::cout << "Hello, usingMemberFunctionAsCompletionHandler!" << std::endl; }
        Printer(boost::asio::io_context& io):
            timer_(io, std::chrono::seconds(1)),
            count_(0)
        {
            timer_.async_wait(std::bind(&Printer::print, this));
            std::cout << "start wait for the timer expiration!" << std::endl;
        }
    };
    boost::asio::io_context io;
    Printer p(io);
    io.run();
}

void synchronisingCompletionHandlersInMultithreadedPrograms()
{
    // 所有计时器共同计时
    class Printer
    {
        // 用于分派handler去执行，如果多个线程访问共享资源，则自动同步获取
        boost::asio::strand<boost::asio::io_context::executor_type> strand_;
        int timer_number_;
        int total_timer_count_max_;
        int count_;
        std::vector<boost::asio::steady_timer> timer_list_;
    public:
        void print(int number)
        {
            if (count_ < total_timer_count_max_)
            {
                std::cout << "Timer" << number+1 << ": " << ++count_ << std::endl;
                // 设置计时器的总过期时间，过期时间从计时器建立就开始计时，因为需要每次加1s，否则会直接过期。
                timer_list_[number].expires_at(timer_list_[number].expiry() + std::chrono::seconds(1));
                timer_list_[number].async_wait(boost::asio::bind_executor(strand_, std::bind(&Printer::print, this, number)));
                // timer_list_[number].async_wait(std::bind(&Printer::print, this, number));
            }
        }
        ~Printer() { std::cout << "Hello, synchronisingCompletionHandlersInMultithreadedPrograms!\ntotal count: " << count_ << std::endl; }
        Printer(boost::asio::io_context& io, int timer_number = 2, int total_timer_count_max = 10):
            strand_(boost::asio::make_strand(io)), 
            timer_number_(timer_number), 
            total_timer_count_max_(total_timer_count_max), 
            count_(0)
        {
            for(int i = 0; i < timer_number_; ++i)
            {
                timer_list_.emplace_back(io, std::chrono::seconds(1));
                // boost::asio::bind_executor生成一个handler通过strand来自动分派所含的handler
                timer_list_.back().async_wait(boost::asio::bind_executor(strand_, std::bind(&Printer::print, this, i)));
                // timer_list_.back().async_wait(std::bind(&Printer::print, this, i));
            }
            std::cout << "start wait for the timer expiration!" << std::endl;
        }
    };
    boost::asio::io_context io;
    // 创建多个异步timer
    Printer p(io, 5, 30);
    // 开始一个新的thread用来io_context::run（无什么作用）
    boost::thread t(boost::bind(&boost::asio::io_context::run, &io));
    io.run();
    t.join();
}

#endif // !BOOST_ASIO_SAMPLE_BASIC_H
