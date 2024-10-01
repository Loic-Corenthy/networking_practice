#pragma once

#include "NetCommon.hpp"
#include "NetTSQueue.hpp"
#include "NetMessage.hpp"

namespace LCNS::Net
{
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
        Connection(Owner owner, asio::io_context& asio_context, asio::ip::tcp::socket&& socket, TSQueue<OwnedMessage<HeaderId_t>>& queue_in)
        : _asio_context(asio_context)
        , _socket(std::move(socket))
        , _message_in_queue(queue_in)
        {
            _owner = owner;
        }

        virtual ~Connection() {

        };

        void connect_to_server(const asio::ip::tcp::resolver::results_type& endpoints)
        {
            // Only clients can connect to a server
            if (_owner != Owner::client)
            {
                return;
            }

            auto work = [this](std::error_code ec,[[maybe_unused]] asio::ip::tcp::endpoint endpoint){

                if (!ec)
                {
                    read_header();
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

        uint32_t client_id() const
        {
            // Simple stuff
            return _client_id;
        }

        void connect_to_client(uint32_t client_id = 0)
        {
            if (_owner == Owner::server)
            {
                if (_socket.is_open())
                {
                    _client_id = client_id;
                    read_header();
                }
            }
        }

    protected:
        asio::io_context& _asio_context;

        asio::ip::tcp::socket _socket;

        TSQueue<Message<HeaderId_t>> _message_out_queue;

        TSQueue<OwnedMessage<HeaderId_t>>& _message_in_queue;

        Owner _owner;

        uint32_t _client_id = 0u;

    private:
        // ASYNC function!
        void read_header()
        {
            auto work = [this](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
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

        // ASYNC function!
        void write_header()
        {
            auto work = [this](std::error_code ec, [[maybe_unused]] std::size_t length)
            {
                if (!ec)
                {
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

        void add_to_incoming_message_queue()
        {
            switch (_owner)
            {
                case Owner::server:
                    _message_in_queue.push_back({ this->shared_from_this(), _tmp_message_in });
                    break;

                case Owner::client:
                    _message_in_queue.push_back({ nullptr, _tmp_message_in });
                    break;
            }

            read_header();
        }

    private:
        Message<HeaderId_t> _tmp_message_in;
    };

}  // namespace LCNS::Net
