#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>
#include <atomic>

using LCNS::ThreadSafe::Queue2;
using std::atomic;
using std::jthread;

TEST_CASE("Empty queue2", "[test][internal]")
{
    GIVEN("An empty queue")
    {
        Queue2<TestData> queue;

        THEN("size returns 0")
        {
            CHECK(queue.size() == 0ul);
            CHECK(queue.is_empty());
        }

        THEN("pop returns false")
        {
            TestData td("empty");

            CHECK_FALSE(queue.try_pop(td));
            CHECK(queue.is_empty());
        }
    }
}

TEST_CASE("Adding elements", "[test][internal]")
{
    GIVEN("An empty queue")
    {
        Queue2<TestData> queue;

        WHEN("Adding an item")
        {
            TestData td1("td1");
            queue.push(std::move(td1));

            THEN("The size increases by 1")
            {
                CHECK(queue.size() == 1ul);
            }

            THEN("We can get back the last item using pop")
            {
                TestData res("empty");

                CHECK(queue.try_pop(res));

                CHECK(res.name == "td1");
                CHECK(queue.is_empty());
            }
        }

        WHEN("Adding multiple items")
        {
            queue.push(TestData("td1"));
            queue.push(TestData("td2"));
            queue.push(TestData("td3"));
            queue.push(TestData("td4"));

            THEN("The size increases accordingly")
            {
                const std::size_t test_size = 4ul;
                CHECK(queue.size() == test_size);
            }

            THEN("We can get back the last item using pop")
            {
                TestData res("empty");

                CHECK(queue.try_pop(res));
                CHECK(queue.try_pop(res));
                CHECK(queue.try_pop(res));
                CHECK(queue.try_pop(res));

                CHECK(res.name == "td4");
                CHECK(queue.is_empty());
            }
        }

        WHEN("Adding and removing elements")
        {
            queue.push(TestData("td1"));
            queue.push(TestData("td2"));
            queue.push(TestData("td3"));
            queue.push(TestData("td4"));

            TestData res("empty");
            queue.try_pop(res);
            queue.try_pop(res);
            queue.try_pop(res);

            queue.push(TestData("td5"));
            queue.push(TestData("td6"));

            THEN("The size increases accordingly")
            {
                const std::size_t test_size = 3ul;
                CHECK(queue.size() == test_size);
            }

            THEN("We can get back the last item using pop")
            {
                queue.try_pop(res);
                CHECK(res.name == "td4");
                CHECK_FALSE(queue.is_empty());
            }
        }
    }
}

TEST_CASE("Clearing the queue", "[test][internal]")
{
    GIVEN("An empty queue")
    {
        Queue2<TestData> queue;

        THEN("It should be possible to call clear")
        {
            CHECK_NOTHROW(queue.clear());

            AND_THEN("The queue should still be empty")
            {
                const std::size_t test_size = 0ul;
                CHECK(queue.size() == test_size);
                CHECK(queue.is_empty());
            }
        }
    }

    GIVEN("A queue with only 1 element in it")
    {
        Queue2<TestData> queue;
        queue.push(TestData("td0"));

        WHEN("Clearing the queue")
        {
            queue.clear();

            THEN("It should be empty")
            {
                const std::size_t test_size = 0ul;
                CHECK(queue.size() == test_size);
                CHECK(queue.is_empty());
            }
        }
    }

    GIVEN("A queue with multiple elements in it")
    {
        Queue2<TestData> queue;

        for (std::size_t i = 0ul; i < 10ul; ++i)
        {
            queue.push(TestData(std::string("td") + std::to_string(i)));
        }

        WHEN("Clearing the queue")
        {
            queue.clear();

            THEN("It should be empty")
            {
                const std::size_t test_size = 0ul;
                CHECK(queue.size() == test_size);
                CHECK(queue.is_empty());
            }
        }
    }
}

TEST_CASE("1 producer - 1 consumer", "[test][single_consumer]")
{
    Queue2<int> tsq2;
    const int   item_count = 1000;

    GIVEN("1000 elements being added to the queue")
    {
        jthread tp1(producer2, std::ref(tsq2), item_count);

        THEN("The same number of elements can be retrieved asynchronously from a different thread using \"try_pop\"")
        {
            auto consumer = [](Queue2<int>& queue, int& tested_items)
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
                jthread tc(consumer, std::ref(tsq2), std::ref(tested_items));
            }

            CHECK(tsq2.is_empty());
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread")
        {
            auto consumer = [](Queue2<int>& queue, int& tested_items)
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
                jthread tc(consumer, std::ref(tsq2), std::ref(tested_items));
            }

            CHECK(tsq2.is_empty());
        }
    }
}

TEST_CASE("5 producers - 1 consumer", "[test][single_consumer]")
{
    Queue2<int> tsq2;
    const int   item_count         = 1000;
    const int   input_thread_count = 5;

    GIVEN("5000 elements being added to the queue")
    {
        jthread producers[input_thread_count];
        for (int i = 0; i < input_thread_count; ++i)
        {
            producers[i] = jthread(producer2, std::ref(tsq2), item_count);
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread using \"try_pop\"")
        {
            auto consumer = [](Queue2<int>& queue, int& tested_items)
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
                jthread tc(consumer, std::ref(tsq2), std::ref(tested_items));
            }

            CHECK(tsq2.is_empty());
        }

        THEN("The same number of elements can be retrieved asynchronously from a different thread")
        {
            auto consumer = [](Queue2<int>& queue, int& tested_items)
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
                jthread tc(consumer, std::ref(tsq2), std::ref(tested_items));
            }

            CHECK(tsq2.is_empty());
        }
    }
}

TEST_CASE("1 consumer - 5 producers", "[test][single_consumer]")
{
    Queue2<int> tsq2;
    const int   item_count         = 1234;
    const int   input_thread_count = 5;

    GIVEN("A thread ready to consume data from the queue")
    {
        auto consumer = [](Queue2<int>& queue, int& tested_items)
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

        jthread tc(consumer, std::ref(tsq2), std::ref(tested_items));

        WHEN("1000 elements are added to the queue by each of the 5 producer threads")
        {
            jthread producers[input_thread_count];
            for (int i = 0; i < input_thread_count; ++i)
            {
                producers[i] = jthread(producer2, std::ref(tsq2), item_count);
            }

            THEN("All the elements are eventually used by the consumer")
            {
                tc.join();

                CHECK(tsq2.is_empty());
            }
        }
    }
}

TEST_CASE("Force stop waiting in a queue", "[test][multi_consumer]")
{
    GIVEN("A queue with less data than consumer threads")
    {
        Queue2<int> tsq2;
        tsq2.push(404);

        const int input_thread_count = 5;
        REQUIRE(tsq2.size() < input_thread_count);

        atomic<int> completed_threads = {};

        WHEN("All the consumer threads start to wait for data to process")
        {
            auto consumer = [](Queue2<int>& queue, atomic<int>& completed_threads)
            {
                static constexpr int invalid = std::numeric_limits<int>::max();
                int                  value   = invalid;
                queue.wait_and_pop(value);
                completed_threads++;
            };

            jthread consumers[input_thread_count];

            for (int i = 0; i < input_thread_count; ++i)
            {
                consumers[i] = jthread(consumer, std::ref(tsq2), std::ref(completed_threads));
            }

            THEN("It's possible to make all the waiting threads to stop even when there is no data.")
            {
                tsq2.stop_waiting();
            }
        }

        CHECK(completed_threads == input_thread_count);
    }
}

TEST_CASE("5 consumers - 5 producers", "[test][multi_consumers]")
{
    Queue2<int> tsq2;
    const int   item_count         = 1000;
    const int   input_thread_count = 5;

    GIVEN("5 threads ready to consume data from the queue")
    {
        auto consumer = [](Queue2<int>& queue, std::atomic<int>& all_tested_items, int& processed_item_per_thread)
        {
            const auto value_count = item_count * input_thread_count;

            while (all_tested_items < value_count)
            {
                static constexpr int invalid = std::numeric_limits<int>::max();
                int                  value   = invalid;

                queue.wait_and_pop(value);
                if (value != invalid)
                {
                    CHECK((-value_count <= value && value <= value_count));
                }

                all_tested_items++;
                processed_item_per_thread++;
            }
        };

        atomic<int> all_tested_items                              = 0;
        int         processed_item_per_thread[input_thread_count] = {};
        jthread     consumers[input_thread_count];

        for (int i = 0; i < input_thread_count; ++i)
        {
            consumers[i] = jthread(consumer, std::ref(tsq2), std::ref(all_tested_items), std::ref(processed_item_per_thread[i]));
        }

        WHEN("1000 elements are added to the queue by each of the 5 producer threads")
        {
            jthread producers[input_thread_count];

            for (int i = 0; i < input_thread_count; ++i)
            {
                producers[i] = jthread(producer2, std::ref(tsq2), item_count);
            }

            while (all_tested_items < item_count * input_thread_count)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(20));
            }

            tsq2.stop_waiting();


            THEN("All the elements are eventually used by the consumer")
            {
                for (int i = 0; i < input_thread_count; ++i)
                {
                    printf("Consumer %d processed %d items\n", i, processed_item_per_thread[i]);
                }

                CHECK(tsq2.is_empty());
            }
        }
    }
}
