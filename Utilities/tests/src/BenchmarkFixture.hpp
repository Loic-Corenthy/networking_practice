#pragma once

#include <cstdio>

class BenchmarkFixture
{
public:
    BenchmarkFixture()
    {
        std::printf("CTEST_FULL_OUTPUT\n");
    }

protected:
    static constexpr int item_per_thread_count = 1024;
    static constexpr int input_thread_count    = 5;
    static constexpr int total_item_count      = item_per_thread_count * input_thread_count;
};


class BenchmarkTSLockFreeStackFixture
{
public:
    BenchmarkTSLockFreeStackFixture()
    {
        std::printf("CTEST_FULL_OUTPUT\n");
    }

protected:
    static constexpr int total_element_count = 30000;
    static constexpr int thread_count        = 20;
};
