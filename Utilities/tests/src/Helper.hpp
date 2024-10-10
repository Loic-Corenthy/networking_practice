#pragma once

#include "TSQueue1.hpp"

inline void producer(LCNS::ThreadSafe::Queue1<int>& queue, const int item_count)
{
    for (int i = 0; i < item_count; ++i)
    {
        queue.push(i);
    }
}
