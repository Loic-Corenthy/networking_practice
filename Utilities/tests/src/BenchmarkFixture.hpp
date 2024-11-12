#pragma once

#include <cstdio>

class BenchmarkQueueFixture
{
public:
    BenchmarkQueueFixture() { std::printf("CTEST_FULL_OUTPUT\n"); }

protected:
    static constexpr int item_per_thread_count = 1024;
    static constexpr int input_thread_count    = 5;
    static constexpr int total_item_count      = item_per_thread_count * input_thread_count;
};
