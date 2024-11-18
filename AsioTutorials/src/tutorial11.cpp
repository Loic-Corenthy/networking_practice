#include <asio.hpp>

#include <array>
#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

using asio::ip::udp;
using namespace std;

string make_daytime()
{
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}

class UDPServer
{
public:
    UDPServer(asio::io_context& io_ctx)
    : _socket(io_ctx, udp::endpoint(udp::v4(), 13))
    {
        start_receive();
    }

private:
    void start_receive()
    {
        _socket.async_receive_from(asio::buffer(_receive_buffer),
                                   _remote_endpoint,
                                   bind(&UDPServer::handle_receive, this, asio::placeholders::error, asio::placeholders::bytes_transferred));
    }

    void handle_receive(const error_code& error, size_t bytes_transferred)
    {
        if (!error)
        {
            cout << "Received  " << bytes_transferred << " bytes from client\n";

            auto message = make_shared<string>(make_daytime());

            _socket.async_send_to(asio::buffer(*message),
                                  _remote_endpoint,
                                  bind(&UDPServer::handle_send, this, message, asio::placeholders::error, asio::placeholders::bytes_transferred));
            start_receive();
        }
    }

    void handle_send(shared_ptr<string> message, const error_code& error, size_t bytes_transferred)
    {
        if (!error)
        {
            cout << "Transferred " << bytes_transferred << " bytes with the message " << *message << '\n';
        }

        cout << "References to message: " << message.use_count() << '\n';
    }

private:
    udp::socket    _socket;
    udp::endpoint  _remote_endpoint;
    array<char, 1> _receive_buffer;
};

int main()
{
    try
    {
        asio::io_context io_ctx;
        UDPServer        server(io_ctx);

        auto run_io_ctx = [&io_ctx]() { io_ctx.run(); };

        auto io_ctx_thread = thread(run_io_ctx);

        cout << "Enter any character to stop\n";
        char d;
        cin >> d;

        io_ctx.stop();
        io_ctx_thread.join();
    }
    catch (const exception& e)
    {
        cerr << e.what() << '\n';
    }

    return EXIT_SUCCESS;
}
