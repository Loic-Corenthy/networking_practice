#include "TSQueue1.hpp"

#include <catch2/catch_test_macros.hpp>
// #include <catch2/generators/catch_generators_random.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>
#include <stdio.h>

using LCNS::ThreadSafe::Queue1;

// using Catch::Generators::random;

using std::jthread;

void producer(Queue1<int>& queue, const int test_count)
{
    printf("Adding %d elements to the queue\n", test_count);

    for (int i = 0; i < test_count; ++i)
    {
        queue.push(i);
    }
}

TEST_CASE("1 producer - 1 consumer", "[queue][mutex]")
{
    Queue1<int> tsq1;
    const int   test_count = 1000;

    GIVEN("1000 elements being added to the queue")
    {
        std::jthread tp1(producer, std::ref(tsq1), test_count);

        THEN("The same number of elements can be retrieved asynchronously from a different thread using \"try_pop\"")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                while (tested_items < test_count)
                {
                    int value = std::numeric_limits<int>::max();
                    if (queue.try_pop(value))
                    {
                        CHECK((-1000 <= value && value <= 1000));
                        tested_items++;
                    }
                }
            };

            int tested_items = 0u;

            {
                std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                while (tested_items < test_count)
                {
                    int value = std::numeric_limits<int>::max();
                    queue.wait_and_pop(value);

                    CHECK((-1000 <= value && value <= 1000));

                    tested_items++;
                }
            };

            int tested_items = 0;

            {
                std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }
    }
}

TEST_CASE("1 consumer - 1 producer", "[queue][mutex]")
{
    Queue1<int> tsq1;
    const int   test_count = 1234;

    GIVEN("A thread ready to consume data from the queue")
    {
        auto consumer = [](Queue1<int>& queue, int& tested_items)
        {
            while (tested_items < test_count)
            {
                int value = std::numeric_limits<int>::max();
                queue.wait_and_pop(value);

                CHECK((-test_count <= value && value <= test_count));

                tested_items++;
            }
        };

        int tested_items = 0;

        std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));

        WHEN("1000 elements are added to the queue")
        {
            std::jthread tp1(producer, std::ref(tsq1), test_count);

            THEN("All the elements are eventually used by the consumer")
            {
                tc.join();

                CHECK(tsq1.is_empty());
            }
        }
    }
}

TEST_CASE("5 producers - 1 consumer")
{
    Queue1<int> tsq1;
    const int   test_count         = 1000;
    const int   input_thread_count = 5;

    GIVEN("5000 elements being added to the queue")
    {
        jthread producers[input_thread_count];
        for (int i = 0; i < input_thread_count; ++i)
        {
            producers[i] = jthread(producer, std::ref(tsq1), test_count);
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread using \"try_pop\"")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                const auto value_count = test_count * input_thread_count;

                while (tested_items < value_count)
                {
                    if (int value = std::numeric_limits<int>::max(); queue.try_pop(value))
                    {
                        CHECK((-value_count <= value && value <= value_count));
                        tested_items++;
                    }
                }
            };

            int tested_items = 0u;

            {
                std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                const auto value_count = test_count * input_thread_count;

                while (tested_items < test_count * input_thread_count)
                {
                    int value = std::numeric_limits<int>::max();
                    queue.wait_and_pop(value);

                    CHECK((-value_count <= value && value <= value_count));

                    tested_items++;
                }
            };

            int tested_items = 0;

            {
                std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }
    }
}


// TEST_CASE("5 producers - 1 consumer", "[queue][mutex]")
// {
//     Queue1<int>       tsq1;
//     const int test_count = 1000;

//     BENCHMARK("1000 elements")
//     {
//         std::jthread tp1(producer, std::ref(tsq1), test_count);
//         std::jthread tp2(producer, std::ref(tsq1), test_count);
//         std::jthread tp3(producer, std::ref(tsq1), test_count);
//         std::jthread tp4(producer, std::ref(tsq1), test_count);
//         std::jthread tp5(producer, std::ref(tsq1), test_count);

//         int          tested_items = 0;
//         std::jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));

//         return tested_items;
//     };
// }

// TEST_CASE("5 producers - 5 consumers", "[queue][mutex]")
// {
//     Queue1<int>       tsq1;
//     const int test_count = 1000;

//     BENCHMARK("1000 elements")
//     {
//         std::jthread tp1(producer, std::ref(tsq1), test_count);
//         std::jthread tp2(producer, std::ref(tsq1), test_count);
//         std::jthread tp3(producer, std::ref(tsq1), test_count);
//         std::jthread tp4(producer, std::ref(tsq1), test_count);
//         std::jthread tp5(producer, std::ref(tsq1), test_count);

//         int          tested_items = 0;
//         std::jthread tc1(consumer, std::ref(tsq1), std::ref(tested_items));
//         std::jthread tc2(consumer, std::ref(tsq1), std::ref(tested_items));
//         std::jthread tc3(consumer, std::ref(tsq1), std::ref(tested_items));
//         std::jthread tc4(consumer, std::ref(tsq1), std::ref(tested_items));
//         std::jthread tc5(consumer, std::ref(tsq1), std::ref(tested_items));

//         return tested_items;
//     };
// }