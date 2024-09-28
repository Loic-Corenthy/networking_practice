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
            undefined,
            server,
            client
        };

    public:
        Connection(Owner owner, asio::io_context& asio_context, asio::ip::tcp::socket socket, TSQueue<OwnedMessage<HeaderId_t>>& queue_in)
        : _asio_context(asio_context)
        , _socket(std::move(socket))
        , _message_in_queue(queue_in)
        {
            _owner = owner;
        }

        virtual ~Connection() {

        };

        bool connect_to_server(const asio::ip::tcp::resolver::results_type& endpoints) { return false; }

        bool disconnect() { return false; }

        bool is_connected() const { return false; }

        bool send([[maybe_unused]] const Message<HeaderId_t>& message) { return false; }

    protected:
        asio::ip::tcp::socket _socket;

        asio::io_context& _asio_context;

        TSQueue<Message<HeaderId_t>> _message_out_queue;

        TSQueue<OwnedMessage<HeaderId_t>>& _message_in_queue;

        Owner _owner = Owner::undefined;
    };

}  // namespace LCNS::Net
