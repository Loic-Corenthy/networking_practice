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
    : ServerInterface(port)
    {
    }

protected:
    bool on_client_connect([[maybe_unused]] std::shared_ptr<Connection<CustomMessageTypes>> client) override { return true; }

    virtual void on_client_disconnect([[maybe_unused]] std::shared_ptr<Connection<CustomMessageTypes>> client) override {}

    virtual void on_message([[maybe_unused]] std::shared_ptr<Connection<CustomMessageTypes>> client,
                            [[maybe_unused]] Message<CustomMessageTypes>&                    message) override
    {
    }
};

bool run = true;

void signal_callback_handler(int signum)
{
    cout << "Caught signal " << signum << '\n';
    run = false;
}

int main()
{
    signal(SIGINT, signal_callback_handler);

    CustomServer server(60000);
    server.start();


    while (run)
    {
        server.update();
        using namespace std::chrono_literals;
        std::this_thread::sleep_for(200ms);
    }

    // server.stop();

    return 0;
}
