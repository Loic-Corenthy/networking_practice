#pragma once

#include "TSQueue1.hpp"
#include "TSQueue2.hpp"

#include <cstdio>
#include <string>

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

    explicit TestData(std::string init_name)
    : name(std::move(init_name))
    {
        printf("Test Data: constructor with name %s\n", name.c_str());
    }

    TestData(const TestData& rhs)
    : name(rhs.name)
    {
        printf("Test Data: copy constructor with name  %s\n", name.c_str());
    }

    TestData(TestData&& rhs)
    : name(std::move(rhs.name))
    {
        printf("Test Data: move constructor  %s\n", name.c_str());
    }

    TestData& operator=(const TestData& rhs)
    {
        name = rhs.name;
        printf("Test Data: copy assignment operator  %s\n", name.c_str());
        return *this;
    }

    TestData& operator=(TestData&& rhs)
    {
        name = std::move(rhs.name);
        printf("Test Data: move assignment operator  %s\n", name.c_str());

        return *this;
    }

    ~TestData() { printf("Test Data: Destructor with name %s\n", name.c_str()); }

    std::string name;
};