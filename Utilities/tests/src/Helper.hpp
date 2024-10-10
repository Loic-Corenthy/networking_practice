#pragma once

#include "TSQueue1.hpp"
#include "TSQueue2.hpp"

#include <cstdio>

inline void producer(LCNS::ThreadSafe::Queue1<int>& queue, const int item_count)
{
    for (int i = 0; i < item_count; ++i)
    {
        queue.push(i);
    }
}

struct TestData
{
    TestData() = delete;

    TestData(int init_index)
    : index(init_index)
    , fl(new float)
    {
        *fl = static_cast<float>(index) * 3.5f;
        printf("Test Data: constructor with index parameter %d and fl: %f\n", index, *fl);
    }

    TestData(const TestData& rhs)
    : index(rhs.index)
    {
        fl  = new float;
        *fl = *rhs.fl;
        printf("Test Data: copy constructor %d and fl: %f\n", index, *fl);
    }

    TestData(TestData&& rhs)
    : index(rhs.index)
    {
        std::swap(fl, rhs.fl);
        printf("Test Data: move constructor %d and fl: %f\n", index, *fl);
    }

    TestData& operator=(const TestData& rhs)
    {
        index = rhs.index;
        fl    = new float;
        *fl   = *rhs.fl;
        printf("Test Data: copy assignment operator%d and fl: %f\n", index, *fl);
        return *this;
    }

    TestData& operator=(TestData&& rhs)
    {
        index = rhs.index;
        std::swap(fl, rhs.fl);
        printf("Test Data: move assignment operator %d and fl: %f\n", index, *fl);

        return *this;
    }

    ~TestData()
    {
        if (fl)
        {
            printf("Test Data: Destructor with index %d and fl: %f\n", index, *fl);
        }
        else
        {
            printf("Test Data: Destructor with index %d and fl: nulltpr\n", index);
        }

        delete fl;
    }

    int    index = 99;
    float* fl    = nullptr;
};