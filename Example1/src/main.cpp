#include "NetCommon.hpp"
#include "NetMessage.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <chrono>



using std::cout;
using std::string;
using std::vector;

void grabSomeData(asio::ip::tcp::socket& socket, vector<char>& buffer)
{
    auto readAvailableData = [&buffer, &socket](std::error_code ec, std::size_t length)
    {
        if (!ec)
        {
            cout << "\n\nRead " << length << " bytes\n\n";

            for (std::size_t i = 0u; i < length; i++)
            {
                cout << buffer.at(i);
            }

            grabSomeData(socket, buffer);
        }
    };

    socket.async_read_some(asio::buffer(buffer.data(), buffer.size()), readAvailableData);
}

enum class HeaderID : uint32_t
{
    Connect
};

int main()
{
    vector<char> generic_buffer(1 * 1024);
    asio::error_code ec;
    asio::io_context context;

    asio::io_context::work idle_work(context);

    auto context_thread = std::thread([&context]() { context.run(); });

    // asio::ip::tcp::endpoint endpoint(asio::ip::make_address("93.184.215.14", ec), 80);
    // asio::ip::tcp::endpoint endpoint(asio::ip::make_address("127.0.0.1", ec), 80);
    asio::ip::tcp::endpoint endpoint(asio::ip::make_address("51.38.81.49", ec), 80);

    asio::ip::tcp::socket socket(context);

    socket.connect(endpoint, ec);

    if (!ec)
    {
        cout << "Connected!\n";
    }
    else
    {
        cout << "Failed to connect to address:\n" << ec.message() << '\n';
    }

    if (socket.is_open())
    {
        grabSomeData(socket, generic_buffer);

        string request = "GET /index.html HTTP/1.1\r\n"
                         "Host: example.com\r\n"
                         "Connection: close\r\n\r\n";

        socket.write_some(asio::buffer(request.data(), request.size()), ec);

        using namespace std::chrono_literals;
        std::this_thread::sleep_for(20s);

        

    }
    
    context.stop();
    
    context_thread.join();

    /* code */

    LCNS::Net::Message<HeaderID> message;

    cout << message;
    return 0;
}
