#include <iostream>
#include <string>
#include <chrono>
#include <functional>
#include <boost/asio.hpp>
#ifdef _WIN32
#include <sdkddkver.h>
// #define WINVER 0x0A00
// #define _WIN32_WINNT 0x0A00
#endif

#ifndef BOOST_ASIO_SAMPLE_H
#define BOOST_ASIO_SAMPLE_H

void usingTimerSynchronously()
{
    boost::asio::io_context io;
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

#endif // !BOOST_ASIO_SAMPLE_H
