#include "TSQueue1.hpp"

#include <catch2/catch_test_macros.hpp>
// #include <catch2/generators/catch_generators_random.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>
#include <stdio.h>

using LCNS::TS::Queue1;

// using Catch::Generators::random;

void producer(Queue1<int>& queue, const std::size_t test_count)
{
    for (std::size_t i = 0; i < test_count; ++i)
    {
        queue.push_back(i);
    }
}

void consumer(Queue1<int>& queue, int& tested_items)
{
    tested_items = 0;
    while (!queue.is_empty())
    {
        const auto val = queue.pop_front();
        CHECK((-1000 <= val && val <= 1000));
        tested_items++;
    }
}

TEST_CASE("1 producer - 1 consumer", "[queue][mutex]")
{
    Queue1<int> tsq1;
    const std::size_t   test_count = 1000;

    BENCHMARK("1000 elements")
    {
        std::jthread tp1(producer, std::ref(tsq1), test_count);

        int          tested_items = 0;
        std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));

        return tested_items;
    };
}


TEST_CASE("5 producers - 1 consumer", "[queue][mutex]")
{
    Queue1<int> tsq1;
    const std::size_t   test_count = 1000;

    BENCHMARK("1000 elements")
    {
        std::jthread tp1(producer, std::ref(tsq1), test_count);
        std::jthread tp2(producer, std::ref(tsq1), test_count);
        std::jthread tp3(producer, std::ref(tsq1), test_count);
        std::jthread tp4(producer, std::ref(tsq1), test_count);
        std::jthread tp5(producer, std::ref(tsq1), test_count);

        int          tested_items = 0;
        std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));

        return tested_items;
    };
}

TEST_CASE("5 producers - 5 consumers", "[queue][mutex]")
{
    Queue1<int> tsq1;
    const std::size_t   test_count = 1000;

    BENCHMARK("1000 elements")
    {
        std::jthread tp1(producer, std::ref(tsq1), test_count);
        std::jthread tp2(producer, std::ref(tsq1), test_count);
        std::jthread tp3(producer, std::ref(tsq1), test_count);
        std::jthread tp4(producer, std::ref(tsq1), test_count);
        std::jthread tp5(producer, std::ref(tsq1), test_count);

        int          tested_items = 0;
        std::jthread tc1(consumer, std::ref(tsq1), std::ref(tested_items));
        std::jthread tc2(consumer, std::ref(tsq1), std::ref(tested_items));
        std::jthread tc3(consumer, std::ref(tsq1), std::ref(tested_items));
        std::jthread tc4(consumer, std::ref(tsq1), std::ref(tested_items));
        std::jthread tc5(consumer, std::ref(tsq1), std::ref(tested_items));

        return tested_items;
    };
}