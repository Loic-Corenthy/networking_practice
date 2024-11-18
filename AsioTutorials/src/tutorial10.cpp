#include <asio.hpp>

#include <array>
#include <ctime>
#include <iostream>
#include <string>

using asio::ip::udp;

using std::array;
using std::cout;
using std::endl;
using std::string;
using std::cin;

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
        asio::io_context io_ctx;

        udp::socket socket(io_ctx, udp::endpoint(udp::v4(), 13));

        bool run = true;
        while (run)
        {
            array<char, 1> receive_buffer = {};
            udp::endpoint  remote_endpoint;
            socket.receive_from(asio::buffer(receive_buffer), remote_endpoint);

            cout << "Received connection request from " << remote_endpoint.address().to_string() << endl;

            string message = make_daytime();

            socket.send_to(asio::buffer(message), remote_endpoint);

            cout << "Keep running?\n";
            char answer;
            cin >> answer;

            run = answer == 'y';
        }
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}