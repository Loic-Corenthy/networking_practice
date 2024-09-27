#pragma once

#include "NetCommon.hpp"

#include <vector>
#include <iostream>
#include <type_traits>

namespace LCNS::Net
{
    template <typename HeaderId_t>
    struct MessageHeader
    {
        HeaderId_t id   = {};
        uint32_t   size = 0u;
    };

    template <typename HeaderId_t>
    struct Message
    {
        MessageHeader<HeaderId_t> header;
        std::vector<uint8_t>      body;

        std::size_t size() const;

        template <typename Data_t>
        Message<HeaderId_t>& operator<<(const Data_t& data)
        {
            static_assert(std::is_standard_layout<Data_t>::value, "Data is too complex to be serialized into a message");

            const auto begin_size = body.size();

            body.resize(begin_size + sizeof(Data_t));

            std::memcpy(body.data() + begin_size, &data, sizeof(Data_t));

            header.size = size();

            return *this;
        }

        template <typename Data_t>
        Message<HeaderId_t>& operator>>(Data_t& data)
        {
            static_assert(std::is_standard_layout<Data_t>::value, "Data is too complex to be serialized into a message");

            const auto begin_size = body.size() - sizeof(Data_t);

            std::memcpy(&data, body.data() + begin_size, sizeof(Data_t));

            body.resize(begin_size);

            header.size = size();

            return *this;
        }
    };

    template <typename HeaderId_t>
    std::size_t Message<HeaderId_t>::size() const
    {
        return sizeof(MessageHeader<HeaderId_t>) + body.size();
    }

    template <typename HeaderId_t>
    std::ostream& operator<<(std::ostream& os, const Message<HeaderId_t>& message)
    {
        os << "ID: " << static_cast<std::underlying_type_t<HeaderId_t>>(message.header.id) << " Size: " << message.size() << '\n';

        return os;
    }

}