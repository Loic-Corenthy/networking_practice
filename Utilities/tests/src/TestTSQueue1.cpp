#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>
#include <stdio.h>

using LCNS::ThreadSafe::Queue1;

using std::atomic;
using std::jthread;

TEST_CASE("1 producer - 1 consumer", "[test][single_consumer]")
{
    Queue1<int> tsq1;
    const int   item_count = 1000;

    GIVEN("1000 elements being added to the queue")
    {
        jthread tp1(producer, std::ref(tsq1), item_count);

        THEN("The same number of elements can be retrieved asynchronously from a different thread using \"try_pop\"")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                while (tested_items < item_count)
                {
                    int value = std::numeric_limits<int>::max();
                    if (queue.try_pop(value))
                    {
                        CHECK((-item_count <= value && value <= item_count));
                        tested_items++;
                    }
                }
            };

            int tested_items = 0u;

            {
                jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                while (tested_items < item_count)
                {
                    int value = std::numeric_limits<int>::max();
                    queue.wait_and_pop(value);

                    CHECK((-item_count <= value && value <= item_count));

                    tested_items++;
                }
            };

            int tested_items = 0;

            {
                jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }
    }
}

TEST_CASE("1 consumer - 1 producer", "[test][single_consumer]")
{
    Queue1<int> tsq1;
    const int   item_count = 1234;

    GIVEN("A thread ready to consume data from the queue")
    {
        auto consumer = [](Queue1<int>& queue, int& tested_items)
        {
            while (tested_items < item_count)
            {
                int value = std::numeric_limits<int>::max();
                queue.wait_and_pop(value);

                CHECK((-item_count <= value && value <= item_count));

                tested_items++;
            }
        };

        int tested_items = 0;

        jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));

        WHEN("1000 elements are added to the queue")
        {
            jthread tp1(producer, std::ref(tsq1), item_count);

            THEN("All the elements are eventually used by the consumer")
            {
                tc.join();

                CHECK(tsq1.is_empty());
            }
        }
    }
}

TEST_CASE("5 producers - 1 consumer", "[test][single_consumer]")
{
    Queue1<int> tsq1;
    const int   item_count         = 1000;
    const int   input_thread_count = 5;

    GIVEN("5000 elements being added to the queue")
    {
        jthread producers[input_thread_count];
        for (int i = 0; i < input_thread_count; ++i)
        {
            producers[i] = jthread(producer, std::ref(tsq1), item_count);
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread using \"try_pop\"")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                const auto value_count = item_count * input_thread_count;

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
                jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread")
        {
            auto consumer = [](Queue1<int>& queue, int& tested_items)
            {
                const auto value_count = item_count * input_thread_count;

                while (tested_items < item_count * input_thread_count)
                {
                    int value = std::numeric_limits<int>::max();
                    queue.wait_and_pop(value);

                    CHECK((-value_count <= value && value <= value_count));

                    tested_items++;
                }
            };

            int tested_items = 0;

            {
                jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));
            }

            CHECK(tsq1.is_empty());
        }
    }
}

TEST_CASE("1 consumer - 5 producers", "[test][single_consumer]")
{
    Queue1<int> tsq1;
    const int   item_count         = 1234;
    const int   input_thread_count = 5;

    GIVEN("A thread ready to consume data from the queue")
    {
        auto consumer = [](Queue1<int>& queue, int& tested_items)
        {
            const auto value_count = item_count * input_thread_count;

            while (tested_items < value_count)
            {
                int value = std::numeric_limits<int>::max();
                queue.wait_and_pop(value);

                CHECK((-value_count <= value && value <= value_count));

                tested_items++;
            }
        };

        int tested_items = 0;

        jthread tc(consumer, std::ref(tsq1), std::ref(tested_items));

        WHEN("1000 elements are added to the queue by each of the 5 producer threads")
        {
            jthread producers[input_thread_count];
            for (int i = 0; i < input_thread_count; ++i)
            {
                producers[i] = jthread(producer, std::ref(tsq1), item_count);
            }

            THEN("All the elements are eventually used by the consumer")
            {
                tc.join();

                CHECK(tsq1.is_empty());
            }
        }
    }
}

TEST_CASE("5 consumers - 5 producers", "[test][multi_consumers]")
{
    Queue1<int> tsq1;
    const int   item_count         = 1000;
    const int   input_thread_count = 5;

    GIVEN("A 5 threads ready to consume data from the queue")
    {
        auto consumer = [](Queue1<int>& queue, std::atomic<int>& all_tested_items, int& processed_item_per_thread)
        {
            const auto value_count = item_count * input_thread_count;

            while (all_tested_items < value_count)
            {
                int value = std::numeric_limits<int>::max();
                if (queue.try_pop(value))
                {
                    CHECK((-value_count <= value && value <= value_count));
                    all_tested_items++;
                    processed_item_per_thread++;
                }
            }
        };

        atomic<int> all_tested_items                              = 0;
        int         processed_item_per_thread[input_thread_count] = {};
        jthread     consumers[input_thread_count];

        for (int i = 0; i < input_thread_count; ++i)
        {
            consumers[i] = jthread(consumer, std::ref(tsq1), std::ref(all_tested_items), std::ref(processed_item_per_thread[i]));
        }

        WHEN("1000 elements are added to the queue by each of the 5 producer threads")
        {
            jthread producers[input_thread_count];

            for (int i = 0; i < input_thread_count; ++i)
            {
                producers[i] = jthread(producer, std::ref(tsq1), item_count);
            }

            THEN("All the elements are eventually used by the consumer")
            {
                for (int i = 0; i < input_thread_count; ++i)
                {
                    if (consumers[i].joinable())
                        consumers[i].join();

                    printf("Consumer %d processed %d items\n", i, processed_item_per_thread[i]);
                }

                CHECK(tsq1.is_empty());
            }
        }
    }
}
