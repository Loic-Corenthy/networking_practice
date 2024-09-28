#pragma once

#include "NetCommon.hpp"
#include "NetConnection.hpp"
#include "NetMessage.hpp"
#include "NetConnection.hpp"
#include "NetTSQueue.hpp"

#include <iostream>
#include <deque>
#include <limits>

namespace LCNS::Net
{
    template <typename HeaderId_t>
    class ServerInterface
    {
    public:
        ServerInterface(uint16_t port)
        : _asio_acceptor(_asio_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
        {
        }

        virtual ~ServerInterface() { stop(); }

        bool start()
        {
            try
            {
                wait_for_client_connection();

                _context_thread = std::thread([this]() { _asio_context.run(); });
            }
            catch (const std::exception& e)
            {
                std::cerr << "[SERVER] Exception: " << e.what() << '\n';
                return false;
            }

            std::cout << "[SERVER] Started!\n";

            return true;
        }

        void stop()
        {
            _asio_context.stop();

            if (_context_thread.joinable())
            {
                _context_thread.join();
            }

            std::cout << "[SERVER] Stopped!\n";
        }

        void wait_for_client_connection()
        {
            auto work = [this](std::error_code ec, asio::ip::tcp::socket socket)
            {
                if (!ec)
                {
                    std::cout << "[SERVER] New connection: " << socket.remote_endpoint() << '\n';

                    // auto new_connection = std::make_shared<Connection<HeaderId_t>>(
                    // Connection<HeaderId_t>::owner::server, _asio_context, std::move(socket), _message_in_queue);

                    // if (on_client_connect(new_connection))
                    // {
                    //     _all_connections.push_back(std::move(new_connection));

                    //     _all_connections.back()->connect_to_client(_id_counter++);

                    //     std::cout << "[" << _all_connections.back()->get_id() << "] Connection approved\n";
                    // }
                    // else
                    // {
                    //     std::cout << "[SERVER] Connection denied\n";
                    // }
                }
                else
                {
                    std::cerr << "[SERVER] New connection error: " << ec.message() << '\n';
                }

                wait_for_client_connection();
            };

            _asio_acceptor.async_accept(work);
        }

        void message_client(std::shared_ptr<Connection<HeaderId_t>> client, const Message<HeaderId_t>& message)
        {
            if (client && client->is_connected())
            {
                client->send(message);
            }
            else
            {
                on_client_disconnect(client);
                client.reset();

                _all_connections.erase(std::remove(_all_connections.begin(), _all_connections.end(), client), _all_connections.end());
            }
        }

        void message_all_clients(const Message<HeaderId_t>& message, std::shared_ptr<Connection<HeaderId_t>> client_to_ignore = nullptr)
        {
            bool are_some_clients_invalid = false;
            for (auto& client : _all_connections)
            {
                if (client && client->is_connected())
                {
                    if (client != client_to_ignore)
                    {
                        client->send(message);
                    }
                }
                else
                {
                    on_client_disconnect(client);
                    client.reset();
                    are_some_clients_invalid = true;
                }
            }

            // DON'T DO THAT WITHIN THE FOR LOOP TO AVOID INVALIDATING THE ITERATORS!
            if (are_some_clients_invalid)
            {
                _all_connections.erase(std::remove(_all_connections.begin(), _all_connections.end(), nullptr), _all_connections.end());
            }
        }

        void update(std::size_t max_messages = std::numeric_limits<std::size_t>::max())
        {
            std::size_t message_count = 0;

            while (message_count < max_messages && !_message_in_queue.is_empty())
            {
                auto message = _message_in_queue.pop_front();

                on_message(message.remote, message.msg);

                message_count++;
            }
        }

    protected:
        virtual bool on_client_connect([[maybe_unused]] std::shared_ptr<Connection<HeaderId_t>> client) { return false; }

        virtual void on_client_disconnect([[maybe_unused]] std::shared_ptr<Connection<HeaderId_t>> client) {}

        virtual void on_message([[maybe_unused]] std::shared_ptr<Connection<HeaderId_t>> client, [[maybe_unused]] Message<HeaderId_t>& message) {}

    protected:
        TSQueue<OwnedMessage<HeaderId_t>> _message_in_queue;

        std::deque<std::shared_ptr<Connection<HeaderId_t>>> _all_connections;

        asio::io_context _asio_context;
        std::thread      _context_thread;

        asio::ip::tcp::acceptor _asio_acceptor;

        uint32_t _id_counter = 10'000u;
    };

}  // namespace LCNS::Net
