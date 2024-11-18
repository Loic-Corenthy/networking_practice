#include <asio.hpp>
#include <iostream>
#include <functional>
#include <array>

using asio::ip::tcp;

int main(int argc, char const* argv[])
{
    try
    {
        if (argc != 2)
        {
            // Can run it as "./boost_tutorial6 localhost" when running ./boost_tutorial7 locally
            std::cerr << "Usage: boost_tutorial6 <host>\n";
            return EXIT_FAILURE;
        }

        asio::io_context io_context;

        tcp::resolver               resolver(io_context);
        tcp::resolver::results_type endpoints = resolver.resolve(argv[1], "daytime");

        tcp::socket socket(io_context);
        asio::connect(socket, endpoints);

        while(true)
        {
            std::array<char, 128> buffer;
            asio::error_code ec;

            size_t length = socket.read_some(asio::buffer(buffer), ec);

            if (ec == asio::error::eof)
            {
                break;
            }
            else if(ec)
            {
                throw asio::system_error(ec);
            }

            std::cout.write(buffer.data(), length);
        }

    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << '\n';
    }

    return 0;
}
