#include "NetCommon.hpp"
#include "NetMessage.hpp"
#include "NetClient.hpp"
#include "NetServer.hpp"

#include <iostream>

enum class CustomMessageType : uint32_t
{
    FireBullet,
    MovePlayer
};

class CustomClient : public LCNS::Net::ClientInterface<CustomMessageType>
{
public:
    bool fire_bullet(float x, float y)
    {
        LCNS::Net::Message<CustomMessageType> message;
        message.header.id = CustomMessageType::FireBullet;

        message << x << y;
        send(message);

        return true;
    }
};

struct Test
{
    float x = {};
    float y = {};
};


int main()
{
    CustomClient client;

    client.connect("community.onelonecoder.com", 60000);
    client.fire_bullet(10.0f, 20.0f);

    return 0;
}
