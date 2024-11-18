#include <asio.hpp>
#include <chrono>
#include <iostream>

using namespace std;

int main()
{
    asio::io_context context;

    asio::steady_timer timer(context, asio::chrono::seconds(3));

    timer.wait();

    cout << "Hello asio!\n";

    return 0;
}
