#define ASIO_STANDALONE
#include <asio.hpp>

#include <array>
#include <iostream>

using asio::ip::udp;

int main(int argc, char const* argv[])
{
    if (argc != 2)
    {
        std::cerr << "Usage: ./boost_tutorial9 <host>\n";
        return EXIT_FAILURE;
    }

    try
    {
        asio::io_context io_ctx;

        udp::resolver resolver(io_ctx);
        udp::endpoint receiver_endpoint = *(resolver.resolve(udp::v4(), argv[1], "daytime").begin());

        udp::socket socket(io_ctx);
        socket.open(udp::v4());

        std::array<char, 1> send_buffer = {};
        socket.send_to(asio::buffer(send_buffer), receiver_endpoint);

        std::array<char, 128> receive_buffer = {};
        udp::endpoint         sender_endpoint;
        const std::size_t     length = socket.receive_from(asio::buffer(receive_buffer), sender_endpoint);

        std::cout.write(receive_buffer.data(), length);

        std::cout << "Sender IP address is " << sender_endpoint.address().to_string() << '\n';
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
