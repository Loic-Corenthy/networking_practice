#include <asio.hpp>
#include <iostream>

using namespace std;

void print(const asio::error_code& ec)
{
    cout << "Output is " << ec.message() << '\n';
}

int main()
{
    asio::io_context context;

    asio::steady_timer timer(context, asio::chrono::seconds(3));
    cout << "Start timer for 3 seconds\n";

    timer.async_wait(&print);

    context.run();

    return 0;
}
