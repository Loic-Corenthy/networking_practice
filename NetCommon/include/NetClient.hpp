#pragma once

#include "NetCommon.hpp"
#include "NetTSQueue.hpp"
#include "NetMessage.hpp"
#include "NetConnection.hpp"

#include <thread>
#include <memory>
#include <iostream>

namespace LCNS::Net
{
    template <typename HeaderId_t>
    class ClientInterface
    {
    public:
        ClientInterface()
        {
        }

        virtual ~ClientInterface() { Disconnect(); }


        bool connect(const std::string& host, const uint16_t port)
        {
            try
            {
                // Create endpoint from full host url
                asio::ip::tcp::resolver resolver(_context);
                auto endpoints = resolver.resolve(host, std::to_string(port));

                // Create connection
                const auto owner = Connection<HeaderId_t>::Owner::client;
                _connection = std::make_unique<Connection<HeaderId_t>>(owner, _context, asio::ip::tcp::socket(_context), _message_in_queue);

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

        void send(const Message<HeaderId_t>& message)
        {
            if (is_connected())
            {
                // std::cout << "Client sending message to connection\n";
                _connection->send(message);
            }
        }

        TSQueue<OwnedMessage<HeaderId_t>>& incoming_queue() { return _message_in_queue; }

    protected:
        asio::io_context _context;

        std::thread _context_thread;

        std::unique_ptr<Connection<HeaderId_t>> _connection;

    private:
        TSQueue<OwnedMessage<HeaderId_t>> _message_in_queue;
    };

}  // namespace LCNS::Net
