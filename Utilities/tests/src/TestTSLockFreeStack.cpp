#include "TSLockFreeStack.hpp"

#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <future>
#include <array>
#include <tuple>
#include <atomic>

using LCNS::ThreadSafe::LockFreeStack;

using std::array;
using std::async;
using std::atomic;
using std::future;
using std::make_tuple;
using std::to_string;
using std::tuple;

TEST_CASE("Basic", "[test][internal]")
{
    const int count = 10;

    GIVEN("An empty stack")
    {
        LockFreeStack<int> stack;

        WHEN(to_string(count) + "Elements are added to it")
        {
            for (int i = 0; i < count; i++)
            {
                stack.push(i);
            }

            THEN("They can be retreived in reverse order")
            {
                for (int i = count - 1; i >= 0; i--)
                {
                    CHECK(*stack.pop() == i);
                }
            }
        }
    }

    GIVEN("An empty stack")
    {
        LockFreeStack<TestData> stack;

        WHEN(to_string(count) + "Elements are added to it")
        {
            TestData test_data("");

            for (int i = 0; i < count; i++)
            {
                test_data.name = to_string(i);
                stack.push(test_data);
            }

            THEN("They can be retreived in reverse order")
            {
                for (int i = count - 1; i >= 0; i--)
                {
                    CHECK(stack.pop()->name == to_string(i));
                }
            }
        }
    }
}

TEST_CASE("Multi threaded", "[test][internal]")
{
    const int total_element_count = 10000;

    GIVEN("A stack with a thread adding elements to it")
    {
        LockFreeStack<int> stack;

        auto insert_elements = [](LockFreeStack<int>& stack, int count) -> bool
        {
            for (int i = 1; i <= count; i++)
            {
                stack.push(i);
            }

            return true;
        };

        future<bool> inserted_done = async(std::launch::async, insert_elements, std::ref(stack), total_element_count);


        WHEN("Other threads retreive those elements")
        {
            auto get_elements = [](LockFreeStack<int>& stack, atomic<bool>& all_inserted) -> tuple<int, int>
            {
                int  count = 0;
                int  sum   = 0;
                auto res   = stack.pop();
                while (res || !all_inserted.load())
                {
                    if (res)
                    {
                        sum += *res;
                        count++;
                    }

                    res = stack.pop();
                }

                return make_tuple(count, sum);
            };

            atomic<bool> are_all_inserted = false;

            future<tuple<int, int>> processed0 = async(std::launch::async, get_elements, std::ref(stack), std::ref(are_all_inserted));
            future<tuple<int, int>> processed1 = async(std::launch::async, get_elements, std::ref(stack), std::ref(are_all_inserted));
            future<tuple<int, int>> processed2 = async(std::launch::async, get_elements, std::ref(stack), std::ref(are_all_inserted));


            THEN("The input is the same as the output (i.e. same count and same total)")
            {
                are_all_inserted.store(inserted_done.get());

                const auto [count0, sum0] = processed0.get();
                const auto [count1, sum1] = processed1.get();
                const auto [count2, sum2] = processed2.get();

                CHECK(count0 + count1 + count2 == total_element_count);
                CHECK(sum0 + sum1 + sum2 == total_element_count * (total_element_count + 1) / 2);
            }
        }
    }
}
