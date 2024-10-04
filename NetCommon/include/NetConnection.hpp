#pragma once

#include "NetCommon.hpp"
#include "NetTSQueue.hpp"
#include "NetMessage.hpp"

#include <iostream>

namespace LCNS::Net
{
    template <typename HeaderId_t>
    class ServerInterface;

    template <typename HeaderId_t>
    class Connection : public std::enable_shared_from_this<Connection<HeaderId_t>>
    {
    public:
        enum class Owner
        {
            server,
            client
        };

    public:
        Connection(Owner owner, asio::io_context& asio_context, asio::ip::tcp::socket socket, TSQueue<OwnedMessage<HeaderId_t>>& queue_in)
        : _asio_context(asio_context)
        , _socket(std::move(socket))
        , _message_in_queue(queue_in)
        {
            std::cout << "Constructor Connection for " << (owner == Owner::server ? "SERVER\n" : "CLIENT\n");
            _owner = owner;

            switch (_owner)
            {
                case Owner::server:
                    _hand_shake_out   = static_cast<uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count());
                    _hand_shake_check = scramble(_hand_shake_out);
                    break;

                case Owner::client:
                    // Nothing to do
                    break;
            }
        }

        virtual ~Connection() {}

        uint32_t client_id() const
        {
            // Simple stuff
            return _client_id;
        }

        void connect_to_client(ServerInterface<HeaderId_t>* server, uint32_t client_id = 0)
        {
            if (_owner == Owner::server)
            {
                if (_socket.is_open())
                {
                    _client_id = client_id;
                    write_validation();
                    read_validation(server);
                    // std::cout << "Connect to client " << _client_id << '\n';
                    // read_header();
                }
            }
        }

        void connect_to_server(const asio::ip::tcp::resolver::results_type& endpoints)
        {
            // Only clients can connect to a server
            if (_owner != Owner::client)
            {
                return;
            }

            auto work = [this](std::error_code ec, [[maybe_unused]] asio::ip::tcp::endpoint endpoint)
            {
                if (!ec)
                {
                    read_validation();
                    // std::cout << " connects to server\n";
                    // read_header();
                }
            };

            asio::async_connect(_socket, endpoints, work);
        }

        void disconnect()
        {
            if (is_connected())
            {
                auto action = [this]() { _socket.close(); };
                asio::post(_asio_context, action);
            }
        }

        bool is_connected() const
        {
            // Just that?
            return _socket.is_open();
        }

        bool send(const Message<HeaderId_t>& message)
        {
            auto work = [this, message]()
            {
                // std::cout << "Sending message to message outgoing queue\n";
                const auto out_queue_is_empty = _message_out_queue.is_empty();
                _message_out_queue.push_back(message);

                // Only add work to the context if it's not already busy
                if (out_queue_is_empty)
                {
                    write_header();
                }
            };

            asio::post(_asio_context, work);


            return true;
        }

        uint64_t scramble(uint64_t input)
        {
            uint64_t output = input ^ 0xDEADBEEF;
            output          = (output & 0xF0F0F0F0) >> 4 | (output & 0x0F0F0F0F) << 4;
            return output ^ 0xC0DEFACE;
        }

    protected:
        asio::io_context& _asio_context;

        asio::ip::tcp::socket _socket;

        TSQueue<Message<HeaderId_t>> _message_out_queue;

        TSQueue<OwnedMessage<HeaderId_t>>& _message_in_queue;

        Message<HeaderId_t> _tmp_message_in;

        Owner _owner;

        uint32_t _client_id = 0u;

        uint64_t _hand_shake_out   = 0;
        uint64_t _hand_shake_in    = 0;
        uint64_t _hand_shake_check = 0;

    private:
        void write_validation()
        {
            auto work = [this](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
                    if (_owner == Owner::client)
                    {
                        read_header();
                    }
                }
                else
                {
                    std::cerr << "Write validation failed\n";
                    _socket.close();
                }
            };

            asio::async_write(_socket, asio::buffer(&_hand_shake_out, sizeof(uint64_t)), work);
        }

        void read_validation(ServerInterface<HeaderId_t>* server = nullptr)
        {
            auto work = [this, server](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
                    switch (_owner)
                    {
                        case Owner::server:
                            if (_hand_shake_in == _hand_shake_check)
                            {
                                std::cout << "Client validated correctly\n";
                                server->on_client_validated(this->shared_from_this());

                                read_header();
                            }
                            else
                            {
                                std::cout << "Client validation failed\n";
                                _socket.close();
                            }

                            break;

                        case Owner::client:
                            _hand_shake_out = scramble(_hand_shake_in);

                            write_validation();
                            break;
                    }
                }
                else
                {
                    std::cerr << "Read validation failed\n";
                    _socket.close();
                }
            };


            asio::async_read(_socket, asio::buffer(&_hand_shake_in, sizeof(uint64_t)), work);
        }

        // ASYNC function!
        void write_header()
        {
            auto work = [this](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
                    // std::cout << "Writing header\n";
                    if (_message_out_queue.front().body.size() > 0u)
                    {
                        write_body();
                    }
                    else
                    {
                        _message_out_queue.pop_front();

                        if (!_message_out_queue.is_empty())
                        {
                            write_header();
                        }
                    }
                }
                else
                {
                    std::cerr << "[" << _client_id << "] Write header failed\n";
                    _socket.close();
                }
            };
            asio::async_write(_socket, asio::buffer(&_message_out_queue.front().header, sizeof(MessageHeader<HeaderId_t>)), work);
        }

        // ASYNC function!
        void write_body()
        {
            auto work = [this](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
                    // std::cout << "Writing body\n";
                    _message_out_queue.pop_front();

                    if (!_message_out_queue.is_empty())
                    {
                        write_header();
                    }
                }
                else
                {
                    std::cerr << "[" << _client_id << "] Write body failed\n";
                    _socket.close();
                }
            };
            asio::async_write(_socket, asio::buffer(_message_out_queue.front().body.data(), _message_out_queue.front().body.size()), work);
        }

        // ASYNC function!
        void read_header()
        {
            auto work = [this](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
                    // std::cout << "Reading header\n";
                    if (_tmp_message_in.header.size > 0u)
                    {
                        _tmp_message_in.body.resize(_tmp_message_in.header.size);
                        read_body();
                    }
                    else
                    {
                        add_to_incoming_message_queue();
                    }
                }
                else
                {
                    std::cerr << "[" << _client_id << "] Read header failed\n";
                    _socket.close();
                }
            };

            asio::async_read(_socket, asio::buffer(&_tmp_message_in.header, sizeof(MessageHeader<HeaderId_t>)), work);
        }

        // ASYNC function!
        void read_body()
        {
            auto work = [this](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
                    // std::cout << "Reading body\n";
                    add_to_incoming_message_queue();
                }
                else
                {
                    std::cerr << "[" << _client_id << "] Read body failed\n";
                    _socket.close();
                }
            };
            asio::async_read(_socket, asio::buffer(_tmp_message_in.body.data(), _tmp_message_in.body.size()), work);
        }

        void add_to_incoming_message_queue()
        {
            switch (_owner)
            {
                case Owner::server:
                    // std::cout << "Server adds message to incoming queue\n";
                    _message_in_queue.push_back({ this->shared_from_this(), _tmp_message_in });
                    break;

                case Owner::client:
                    // std::cout << "Client adds message to incoming queue\n";
                    _message_in_queue.push_back({ nullptr, _tmp_message_in });
                    break;
            }

            read_header();
        }
    };

}  // namespace LCNS::Net
