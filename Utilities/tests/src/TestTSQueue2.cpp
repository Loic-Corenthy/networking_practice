#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>

using LCNS::ThreadSafe::Queue2;

TEST_CASE("Empty queue2","[test][internal]")
{
    GIVEN("An empty queue")
    {
        Queue2<TestData> queue;

        THEN("size returns 0")
        {
            CHECK(queue.size() == 0ul);
        }

        THEN("pop returns false")
        {
            TestData td(0);

            CHECK_FALSE(queue.try_pop(td));
        }
    }
}

TEST_CASE("Adding elements","[test][internal]")
{
    GIVEN("An empty queue")
    {
        Queue2<TestData> queue;

        WHEN("Adding an item")
        {
            TestData td1(1);
            queue.push(std::move(td1));

            THEN("The size increases by 1")
            {
                CHECK(queue.size() == 1ul);
            }

            THEN("We can get back the last item using pop")
            {
                TestData res(0);

                CHECK(queue.try_pop(res));

                CHECK(res.index == 1);
                CHECK(*res.fl == 3.5f);
            }
        }

        WHEN("Adding multiple items")
        {
            queue.push(TestData(1));
            queue.push(TestData(2));
            queue.push(TestData(3));
            queue.push(TestData(4));

            THEN("The size increases accordingly")
            {
                CHECK(queue.size() == 4ul);
            }

            THEN("We can get back the last item using pop")
            {
                TestData res(0);

                CHECK(queue.try_pop(res));
                CHECK(queue.try_pop(res));
                CHECK(queue.try_pop(res));
                CHECK(queue.try_pop(res));

                CHECK(res.index == 4);
                CHECK(*res.fl == 4.0f * 3.5f);
            }
        }
    }

}