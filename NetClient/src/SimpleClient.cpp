#include "NetCommon.hpp"
#include "NetMessage.hpp"
#include "NetClient.hpp"
#include "NetServer.hpp"

#include <iostream>
#include <thread>
#include <chrono>
#include <atomic>

using std::cin;
using std::cout;
using std::thread;

enum class CustomMessageType : uint32_t
{
    server_accept,
    server_deny,
    server_ping,
    message_all,
    server_message
};

class CustomClient : public LCNS::Net::ClientInterface<CustomMessageType>
{
public:
    void ping_server()
    {
        LCNS::Net::Message<CustomMessageType> message;
        message.header.id = CustomMessageType::server_ping;

        const auto now = std::chrono::steady_clock::now();
        message << now;
        send(message);
    }
};

struct Test
{
    float x = {};
    float y = {};
};

void keypress(std::atomic<bool>& run, CustomClient& client)
{
    while (run.load())
    {
        cout << "Enter command:\n";
        char ch;
        cin >> ch;

        switch (ch)
        {
            case 'q':
                run.store(false);
                break;

            case '1':
                cout << "Trying to ping server\n";
                client.ping_server();
                break;

            case '2':

                break;

            case '3':

                break;

            default:
                break;
        }
    }
}

int main()
{
    CustomClient client;

    client.connect("127.0.0.1", 60000);

    std::atomic<bool> run = true;

    auto user_input_thread = thread(keypress, std::ref(run), std::ref(client));

    while (run)
    {
        if (client.is_connected())
        {
            if (!client.incoming_queue().is_empty())
            {
                auto message = client.incoming_queue().front().msg;

                switch (message.header.id)
                {
                    case CustomMessageType::server_ping:
                    {
                        const auto                            now = std::chrono::steady_clock::now();
                        std::chrono::steady_clock::time_point then;
                        message >> then;

                        cout << "Time to reply is " << std::chrono::duration<double>(now - then).count() << '\n';
                    }
                    break;

                    default:
                        break;
                }
            }
        }
        else
        {
            cout << "Server down!\n";
            run = false;
        }
    }

    user_input_thread.join();

    cout << "Bye!\n";
    return 0;
}
