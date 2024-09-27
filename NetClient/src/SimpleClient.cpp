#include "NetCommon.hpp"
#include "NetMessage.hpp"

#include <iostream>

enum class CustomMessageType : uint32_t
{
    FireBullet,
    MovePlayer
};

struct Test
{
    float x = {};
    float y = {};
};


int main()
{
    LCNS::Net::Message<CustomMessageType> message;

    message.header.id = CustomMessageType::FireBullet;

    int a = 1;
    bool b = true;
    float c = 3.141592f;

    Test d[5];

    for (std::size_t i = 0; i < 5; ++i)
    {
        d[i].x = static_cast<float>(i+1);
        d[i].y = static_cast<float>(i*2);
    }

    message << a << b << c << d;

    int aa = 0;
    bool bb = false;
    float cc = 1.78f;
    Test dd[5];
    message >> dd >> cc >> bb >> aa;
    return 0;
}
