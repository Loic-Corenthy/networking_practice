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

        const auto now = std::chrono::system_clock::now();

        // cout << "Before sending the message " << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() << '\n';

        const auto value = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();

        message << value;
        send(message);
    }

    void message_all()
    {
        LCNS::Net::Message<CustomMessageType> message;
        message.header.id = CustomMessageType::message_all;

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
                cout << "Sending message to all other clients\n";
                client.message_all();
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
                auto message = client.incoming_queue().pop_front().msg;

                switch (message.header.id)
                {
                    case CustomMessageType::server_accept:
                        cout << "Server has accepted connection\n";
                        break;

                    case CustomMessageType::server_ping:
                    {
                        const auto now = std::chrono::system_clock::now();
                        int64_t    then;
                        message >> then;

                        cout << "Ping:  " << std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() - then << '\n';
                    }
                    break;

                    case CustomMessageType::server_message:
                    {
                        uint32_t id;
                        message >> id;
                        cout << "Hello from client [" << id << "]\n";
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
