#include <asio.hpp>
#include <iostream>
#include <functional>

void print(const asio::error_code& , asio::steady_timer* timer, int* count)
{
    if (*count < 5)
    {
        std::cout << "Count is  " << *count << '\n';
        timer->expires_from_now(asio::chrono::seconds(1));
        *count += 1;

        timer->async_wait(std::bind(&print, asio::placeholders::error, timer, count));
    }
}

int main()
{
    asio::io_context context;

    asio::steady_timer timer(context, asio::chrono::seconds(1));
    std::cout << "Start timer for 5 seconds\n";

    int count = 0;
    timer.async_wait(std::bind(&print, asio::placeholders::error, &timer, &count));

    context.run();

    std::cout << "Final count is " << count << '\n';

    return 0;
}
