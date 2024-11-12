#include "TSList1.hpp"
#include "BenchmarkFixture.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/benchmark/catch_benchmark.hpp>

#include <thread>
#include <stdio.h>
#include <list>
#include <algorithm>

using LCNS::ThreadSafe::List1;

using std::jthread;

TEST_CASE_METHOD(BenchmarkListFixture, "Baseline", "[benchmark]")
{
    BENCHMARK("Single thread with std::list")
    {
        std::list<int> list;

        for (int i = 0; i < BenchmarkListFixture::input_size; ++i)
        {
            list.push_front(i);
        }

        int target       = 0;
        int found_target = 0;

        auto find_target = [&target](const int i) { return i == target; };

        for (int i = 0; i < BenchmarkListFixture::input_size; i++)
        {
            auto it = std::find_if(list.begin(), list.end(), find_target);
            if (it != list.end())
            {
                found_target++;
            }
        }

        CHECK(found_target == BenchmarkListFixture::input_size);

        return 0;
    };


    BENCHMARK("Single thread with List1")
    {
        List1<int> list;

        for (int i = 0; i < BenchmarkListFixture::input_size; ++i)
        {
            list.push_front(i);
        }

        int target       = 0;
        int found_target = 0;

        auto find_target = [&target](const int i) { return i == target; };

        for (int i = 0; i < BenchmarkListFixture::input_size; i++)
        {
            if (list.find_first_if(find_target))
            {
                found_target++;
            }
        }

        CHECK(found_target == BenchmarkListFixture::input_size);

        return 0;
    };
}

TEST_CASE_METHOD(BenchmarkListFixture, "Mutilple threads reading", "[benchmark][multi-consumers]")
{
    BENCHMARK("std::list with 5 consumer threads")
    {
        std::list<int> list;

        for (int i = 0; i < BenchmarkListFixture::input_size; ++i)
        {
            list.push_front(i);
        }

        std::mutex list_mutex;

        auto find_range = [&list_mutex](std::list<int>& list, int begin_value, int target_count)
        {
            int found_count = 0;
            int target      = begin_value;

            auto in_range = [&target](const int i) -> bool { return i == target; };

            while (found_count < target_count)
            {
                std::scoped_lock lock(list_mutex);

                auto it = std::find_if(list.begin(), list.end(), in_range);
                if (it != list.end())
                {
                    target++;
                    found_count++;
                }
            }

            CHECK(found_count == target_count);
        };

        jthread t4 = jthread(find_range, std::ref(list), 800, item_per_thread);
        jthread t3 = jthread(find_range, std::ref(list), 600, item_per_thread);
        jthread t2 = jthread(find_range, std::ref(list), 400, item_per_thread);
        jthread t1 = jthread(find_range, std::ref(list), 200, item_per_thread);
        jthread t0 = jthread(find_range, std::ref(list), 0, item_per_thread);

        return 0;
    };

    BENCHMARK("List1 5 consumer threads")
    {
        List1<int> list;

        for (int i = 0; i < BenchmarkListFixture::input_size; ++i)
        {
            list.push_front(i);
        }

        auto find_range = [](List1<int>& list, int begin_value, int target_count)
        {
            int found_count = 0;
            int target      = begin_value;

            auto in_range = [&target](const int i) -> bool { return i == target; };

            while (found_count < target_count)
            {
                if (list.find_first_if(in_range))
                {
                    target++;
                    found_count++;
                }
            }

            CHECK(found_count == target_count);
        };

        jthread t4 = jthread(find_range, std::ref(list), 800, item_per_thread);
        jthread t3 = jthread(find_range, std::ref(list), 600, item_per_thread);
        jthread t2 = jthread(find_range, std::ref(list), 400, item_per_thread);
        jthread t1 = jthread(find_range, std::ref(list), 200, item_per_thread);
        jthread t0 = jthread(find_range, std::ref(list), 0, item_per_thread);

        return 0;
    };
}