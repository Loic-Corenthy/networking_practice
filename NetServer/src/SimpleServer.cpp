#include <NetCommon.hpp>
#include <NetServer.hpp>
#include <NetConnection.hpp>
#include <NetMessage.hpp>

#include <signal.h>
#include <iostream>

using LCNS::Net::Connection;
using LCNS::Net::Message;
using LCNS::Net::ServerInterface;

using std::cout;

enum class CustomMessageTypes : uint32_t
{
    server_accept,
    server_deny,
    server_ping,
    message_all,
    server_message
};

class CustomServer : public ServerInterface<CustomMessageTypes>
{
public:
    CustomServer(uint16_t port)
    : ServerInterface<CustomMessageTypes>(port)
    {
    }

protected:
    bool on_client_connect([[maybe_unused]] std::shared_ptr<Connection<CustomMessageTypes>> client) override { return true; }

    virtual void on_client_disconnect([[maybe_unused]] std::shared_ptr<Connection<CustomMessageTypes>> client) override {}

    virtual void on_message(std::shared_ptr<Connection<CustomMessageTypes>> client, Message<CustomMessageTypes>& message) override
    {
        switch (message.header.id)
        {
            case CustomMessageTypes::server_ping:
                std::cout << "[SERVER] Client: " << client->client_id() << " Server ping\n";
                client->send(message);
                break;

            default:
                break;
        }
    }
};

std::atomic<bool> run = true;

void signal_callback_handler(int signum)
{
    cout << "\nCaught signal " << signum << '\n';
    run.store(false);
}

int main()
{
    signal(SIGINT, signal_callback_handler);

    CustomServer server(60000);
    server.start();


    while (run.load())
    {
        server.update(10'000'000u, true);
    }

    // server.stop();

    return 0;
}
