#define ASIO_STANDALONE
#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>

using asio::ip::tcp;
using std::string;

string make_daytime()
{
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}

int main()
{     
    try
    {
        asio::io_context io_context;

        tcp::acceptor acceptor(io_context, tcp::endpoint(tcp::v4(), 13));

        bool run = true;
        while(run)
        {
            std::cout << "Waiting for connection...\n";

            tcp::socket socket(io_context);
            acceptor.accept(socket);

            std::cout << "Socket accepted\n";

            std::string message = make_daytime();

            std::error_code ignored_error;
            asio::write(socket, asio::buffer(message), ignored_error);
            
            std::cout << "Keep running?\n";
            char answer;
            std::cin >> answer;

            run = answer == 'y';
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    std::cout <<  "Bye!\n";
    return EXIT_SUCCESS;
}