#include <asio.hpp>
#include <ctime>
#include <iostream>
#include <string>

using asio::ip::tcp;
using namespace std;

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
        while (run)
        {
            cout << "Waiting for connection...\n";

            tcp::socket socket(io_context);
            acceptor.accept(socket);

            cout << "Socket accepted\n";

            string message = make_daytime();

            error_code ignored_error;
            asio::write(socket, asio::buffer(message), ignored_error);

            cout << "Keep running?\n";
            char answer;
            cin >> answer;

            run = answer == 'y';
        }
    }
    catch (const exception& e)
    {
        cerr << e.what() << '\n';
    }

    cout << "Bye!\n";
    return EXIT_SUCCESS;
}
