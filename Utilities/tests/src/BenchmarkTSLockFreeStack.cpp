#include "TSLockFreeStack.hpp"
#include "Helper.hpp"
#include "BenchmarkFixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>
#include <stack>
#include <mutex>
#include <future>
#include <vector>
#include <numeric>

using LCNS::ThreadSafe::LockFreeStack;

using namespace std;

namespace
{
    template <typename Data>
    class TestTSStack
    {
        std::stack<Data> stack;
        std::mutex       mutex;

    public:
        void push(const Data& data)
        {
            std::scoped_lock lock(mutex);
            stack.push(data);
        }

        Data pop()
        {
            std::scoped_lock lock(mutex);
            if (stack.empty())
            {
                return {};
            }

            const auto result = stack.top();
            stack.pop();
            return result;
        }
    };

    template <typename STACK>
    future<bool> set_all_elements(STACK& stack, const long total_element_count)
    {
        auto insert_elements = [](STACK& stack, long count) -> bool
        {
            for (long i = 1; i <= count; i++)
            {
                stack.push(i);
            }

            return true;
        };

        return async(std::launch::async, insert_elements, std::ref(stack), total_element_count);
    }

    template <typename STACK>
    void get_all_elements(STACK& stack, future<bool>&& inserted_done, const long thread_count, const long total_element_count)
    {
        auto get_elements = [](STACK& stack, atomic<bool>& all_inserted) -> tuple<long, long>
        {
            long  count = 0;
            long  sum   = 0;
            auto res   = stack.pop();
            while (res || !all_inserted.load())
            {
                if (res)
                {
                    if constexpr (std::is_same_v<STACK, TestTSStack<long>>)
                    {
                        sum += res;
                    }
                    else if constexpr (std::is_same_v<STACK, LockFreeStack<long>>)
                    {
                        sum += *res;
                    }
                    else
                    {
                        static_assert(false);
                    }

                    count++;
                }

                res = stack.pop();
            }

            return make_tuple(count, sum);
        };


        atomic<bool> are_all_inserted = false;

        vector<future<tuple<long, long>>> results;
        for (long i = 0; i < thread_count; ++i)
        {
            results.push_back(async(std::launch::async, get_elements, std::ref(stack), std::ref(are_all_inserted)));
        }

        are_all_inserted.store(inserted_done.get());

        vector<long> partial_counts;
        vector<long> partial_sums;
        for (auto& futures : results)
        {
            const auto [count, sum] = futures.get();
            partial_counts.push_back(count);
            partial_sums.push_back(sum);
        }

        const long total_count = accumulate(partial_counts.begin(), partial_counts.end(), 0);
        const long total_sum   = accumulate(partial_sums.begin(), partial_sums.end(), 0);

        CHECK(total_count == total_element_count);
        CHECK(total_sum == total_element_count * (total_element_count + 1) / 2);
    }
}

TEST_CASE_METHOD(BenchmarkTSLockFreeStackFixture, "Baseline with TestTSStack implementation", "[benchmark]")
{
    BENCHMARK("TestTSStack with " + to_string(thread_count) + " threads and " + to_string(total_element_count) + " values")
    {
        TestTSStack<long> stack;

        future<bool> inserted_done = set_all_elements(stack, total_element_count);

        get_all_elements(
        stack, std::move(inserted_done), thread_count, total_element_count);

        return 0;
    };

    BENCHMARK("LockFreeStack with " + to_string(thread_count) + " threads and " + to_string(total_element_count) + " values")
    {
        LockFreeStack<long> stack;

        future<bool> inserted_done = set_all_elements(stack, total_element_count);

        get_all_elements(
        stack, std::move(inserted_done), thread_count, total_element_count);

        return 0;
    };
}
