#pragma once

#include "NetCommon.hpp"
#include "NetTSQueue.hpp"
#include "NetMessage.hpp"
#include "NetConnection.hpp"

#include <thread>
#include <memory>

namespace LCNS::Net
{
    template <typename HeaderId_t>
    class ClientInterface
    {
    public:
        ClientInterface()
        : _socket(_context)
        {
        }

        virtual ~ClientInterface() { Disconnect(); }


        bool connect([[maybe_unused]] const std::string& host, [[maybe_unused]] const uint16_t port)
        {
            try
            {
                // Create connection
                _connection = std::make_unique<Connection<HeaderId_t>>();  // TODO

                asio::ip::tcp::resolver resolver(_context);

                auto endpoints = resolver.resolve(host, std::to_string(port));

                _connection->connect_to_server(endpoints);

                _context_thread = std::thread([this]() { _context.run(); });
            }
            catch (const std::exception& e)
            {
                std::cerr << "Client Exception" << e.what() << '\n';
                return false;
            }


            return false;
        }

        void Disconnect()
        {
            if (is_connected())
            {
                _connection->disconnect();
            }

            _context.stop();

            if (_context_thread.joinable())
            {
                _context_thread.join();
            }

            _connection.reset();
        }

        bool is_connected() const
        {
            if (_connection)
            {
                return _connection->is_connected();
            }

            return false;
        }

        TSQueue<OwnedMessage<HeaderId_t>>& incoming_queue() { return _message_in_queue; }

    protected:
        asio::io_context _context;

        std::thread _context_thread;

        asio::ip::tcp::socket _socket;

        // asio::ip::tcp::endpoint _endpoint;

        std::unique_ptr<Connection<HeaderId_t>> _connection;

    private:
        TSQueue<OwnedMessage<HeaderId_t>> _message_in_queue;
    };

}  // namespace LCNS::Net
