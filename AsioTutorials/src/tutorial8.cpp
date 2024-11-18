#include <asio.hpp>

#include <ctime>
#include <functional>
#include <iostream>
#include <memory>
#include <string>

using asio::ip::tcp;

using std::enable_shared_from_this;
using std::make_shared;
using std::shared_ptr;
using std::string;

string make_daytime()
{
    using namespace std;
    time_t now = time(0);
    return ctime(&now);
}

class TCPConnection : public enable_shared_from_this<TCPConnection>
{
public:
    static shared_ptr<TCPConnection> create(asio::io_context& io_context)
    {
        std::cout << "Create new connection\n";
        return shared_ptr<TCPConnection>(new TCPConnection(io_context));
    }

    tcp::socket& get_socket()
    {
        return _socket;
    }

    void start()
    {
        std::cout << "New connection: start\n";
        _message = make_daytime();

        asio::async_write(
        _socket,
        asio::buffer(_message),
        std::bind(&TCPConnection::handle_write, shared_from_this(), asio::placeholders::error, asio::placeholders::bytes_transferred));
    }

private:
    TCPConnection(asio::io_context& io_context)
    : _socket(io_context)
    {
    }

    void handle_write(const asio::error_code& ec, std::size_t byte_transferred)
    {
        if (!ec)
        {
            std::cout << "Transferred " << byte_transferred << " bytes\n";
        }
    }

private:
    tcp::socket _socket;
    string      _message;
};

class TCPServer
{
public:
    TCPServer(asio::io_context& context)
    : _context(context)
    , _acceptor(context, tcp::endpoint(tcp::v4(), 13))
    {
        start_accept();
    }

private:
    void start_accept()
    {
        auto new_connection = TCPConnection::create(_context);

        _acceptor.async_accept(new_connection->get_socket(), std::bind(&TCPServer::handle_accept, this, new_connection, asio::placeholders::error));
    }

    void handle_accept(std::shared_ptr<TCPConnection> new_connection, const asio::error_code& ec)
    {
        if (!ec)
        {
            new_connection->start();
        }

        start_accept();
    }

private:
    asio::io_context& _context;
    tcp::acceptor     _acceptor;
};

int main()
{
    try
    {
        asio::io_context io_context;
        TCPServer        server(io_context);
        io_context.run();
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }


    return EXIT_SUCCESS;
}