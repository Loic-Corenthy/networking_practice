#include <asio.hpp>
#include <chrono>
#include <iostream>

int main()
{
    asio::io_context context;

    asio::steady_timer timer(context, std::chrono::seconds(3));

    timer.wait();

    std::cout << "Hello asio!\n";

    return 0;
}
