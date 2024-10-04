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
    bool on_client_connect([[maybe_unused]] std::shared_ptr<Connection<CustomMessageTypes>> client) override
    {
        Message<CustomMessageTypes> message;
        message.header.id = CustomMessageTypes::server_accept;
        client->send(message);

        return true;
    }

    virtual void on_client_disconnect([[maybe_unused]] std::shared_ptr<Connection<CustomMessageTypes>> client) override
    {
        cout << "Removing client [" << client->client_id() << "]\n";
    }

    virtual void on_message(std::shared_ptr<Connection<CustomMessageTypes>> client, Message<CustomMessageTypes>& message) override
    {
        switch (message.header.id)
        {
            case CustomMessageTypes::server_ping:
                std::cout << "[SERVER] Client: " << client->client_id() << " Server ping\n";
                // client->send(message);

                message_client(client, message);
                break;

            case CustomMessageTypes::message_all:
            {
                std::cout << "[SERVER] Client: " << client->client_id() << " send message to all other clients\n";

                Message<CustomMessageTypes> message_to_client;
                message_to_client.header.id = CustomMessageTypes::server_message;
                message_to_client << client->client_id();
                message_all_clients(message_to_client, client);
            }

            break;


            default:
                break;
        }
    }
};

std::atomic<bool> run = true;
CustomServer      server(60000);

void signal_callback_handler(int signum)
{
    cout << "\nCaught signal " << signum << '\n';
    run.store(false);
    server.force_stop_waiting_in_queue();
}

int main()
{
    signal(SIGINT, signal_callback_handler);

    server.start();


    while (run.load())
    {
        server.update(10'000'000u, true);
    }


    return 0;
}
