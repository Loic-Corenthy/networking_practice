#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using LCNS::ThreadSafe::Queue2;
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

            THEN("The size increases by 1") { CHECK(queue.size() == 1ul); }

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
                    // int value = std::numeric_limits<int>::max();
                    // queue.wait_and_pop(value);

                    // CHECK((-item_count <= value && value <= item_count));

                    // tested_items++;

                    int value = std::numeric_limits<int>::max();
                    if (queue.try_pop(value))
                    {
                        CHECK((-item_count <= value && value <= item_count));
                        tested_items++;
                    }
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