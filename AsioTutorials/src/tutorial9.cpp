#include <asio.hpp>

#include <array>
#include <iostream>

using asio::ip::udp;
using namespace std;

int main(int argc, char const* argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: ./boost_tutorial9 <host>\n";
        return EXIT_FAILURE;
    }

    try
    {
        asio::io_context io_ctx;

        udp::resolver resolver(io_ctx);
        udp::endpoint receiver_endpoint = *(resolver.resolve(udp::v4(), argv[1], "daytime").begin());

        udp::socket socket(io_ctx);
        socket.open(udp::v4());

        array<char, 1> send_buffer = {};
        socket.send_to(asio::buffer(send_buffer), receiver_endpoint);

        array<char, 128> receive_buffer = {};
        udp::endpoint         sender_endpoint;
        const size_t     length = socket.receive_from(asio::buffer(receive_buffer), sender_endpoint);

        cout.write(receive_buffer.data(), length);

        cout << "Sender IP address is " << sender_endpoint.address().to_string() << '\n';
    }
    catch (const exception& e)
    {
        cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
