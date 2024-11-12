#include "Helper.hpp"
#include "BenchmarkFixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>

using LCNS::ThreadSafe::Queue2;

using std::atomic;
using std::jthread;

TEST_CASE_METHOD(BenchmarkQueueFixture, "1 consumer - 5 producers", "[benchmark][single_consumer]")
{
    BENCHMARK("1 consumers - 5 producers")
    {
        Queue2<int> tsq2;

        auto consumer = [](Queue2<int>& queue)
        {
            int tested_items = 0;

            while (tested_items < total_item_count)
            {
                int value = std::numeric_limits<int>::max();
                if (queue.try_pop(value))
                {
                    CHECK((-item_per_thread_count <= value && value <= item_per_thread_count));
                    tested_items++;
                }
            }
        };

        jthread consumer_thread(consumer, std::ref(tsq2));

        jthread producers[input_thread_count];

        for (int i = 0; i < input_thread_count; ++i)
        {
            producers[i] = jthread(producer2, std::ref(tsq2), item_per_thread_count);
        }

        consumer_thread.join();
        CHECK(tsq2.is_empty());
        return 0;
    };
}

TEST_CASE_METHOD(BenchmarkQueueFixture, "5 consumers - 5 producers", "[benchmark][multi_consumers]")
{
    BENCHMARK("5 consumers - 5 producers")
    {
        Queue2<int> tsq2;
        atomic<int> all_tested_items = 0;

        auto consumer = [](Queue2<int>& queue, std::atomic<int>& all_tested_items)
        {
            // Probably not the best condition but it works for now
            while (all_tested_items < total_item_count)
            {
                int value = std::numeric_limits<int>::max();
                if (queue.try_pop(value))
                {
                    CHECK((-total_item_count <= value && value <= total_item_count));
                    all_tested_items++;
                }
            }
        };

        {
            jthread consumers[input_thread_count];

            for (int i = 0; i < input_thread_count; ++i)
            {
                consumers[i] = jthread(consumer, std::ref(tsq2), std::ref(all_tested_items));
            }

            jthread producers[input_thread_count];

            for (int i = 0; i < input_thread_count; ++i)
            {
                producers[i] = jthread(producer2, std::ref(tsq2), item_per_thread_count);
            }
        }

        CHECK(tsq2.is_empty());

        return 0;
    };
}
