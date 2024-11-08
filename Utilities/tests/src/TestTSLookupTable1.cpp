#include "TSLookupTable1.hpp"

#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>
#include <vector>

using LCNS::ThreadSafe::LookupTable1;

using std::atomic;
using std::jthread;
using std::string;
using std::to_string;
using std::vector;

class TestLookupTable1Fixture
{
public:
    TestLookupTable1Fixture()
    : default_test_data(default_name)
    {
    }

    vector<string> generate_test_names(const std::size_t count)
    {
        vector<string> test_names;

        for (std::size_t i = 0; i < count; ++i)
        {
            test_names.push_back("test data " + to_string(i));
        }

        return test_names;
    }

protected:
    const string   default_name = "empty_default";
    const TestData default_test_data;
};

TEST_CASE_METHOD(TestLookupTable1Fixture, "Empty lookup_table", "[test]")
{
    GIVEN("An empty lookup_table")
    {
        LookupTable1<string, TestData> lookup_table;

        WHEN("Trying to get any value")
        {
            const auto result = lookup_table.value_for("", default_test_data);

            THEN("The lookup_table returns the default provided value")
            {
                CHECK(result.name == default_name);
            }
        }
    }
}

TEST_CASE_METHOD(TestLookupTable1Fixture, "Add and retrieve elements", "[test]")
{
    const auto test_names = generate_test_names(5);

    GIVEN("A lookup_table with items")
    {
        LookupTable1<string, TestData> lookup_table;

        for (auto& test_name : test_names)
        {
            lookup_table.add_or_update_mapping(test_name, TestData(test_name));
        }

        THEN("Trying to get the values back returns the correct objects")
        {
            for (const auto& test_name : test_names)
            {
                const auto result = lookup_table.value_for(test_name, default_test_data);
                CHECK(result.name == test_name);
            }
        }
    }
}

TEST_CASE_METHOD(TestLookupTable1Fixture, "Add and remove elements", "[test]")
{
    const auto test_names = generate_test_names(5);

    GIVEN("A lookup_table with items")
    {
        LookupTable1<string, TestData> lookup_table;

        for (auto& test_name : test_names)
        {
            lookup_table.add_or_update_mapping(test_name, TestData(test_name));
        }

        WHEN("All the items in the lookup_table are deleted")
        {
            for (const auto& test_name : test_names)
            {
                lookup_table.remove_mapping(test_name);
            }

            THEN("We get the default value when trying to get them again")
            {
                for (const auto& test_name : test_names)
                {
                    const auto result = lookup_table.value_for(test_name, default_test_data);
                    CHECK(result.name == default_name);
                }
            }
        }
    }
}

TEST_CASE_METHOD(TestLookupTable1Fixture, "Read from multiple threads", "[test][multi_consumer]")
{
    GIVEN("A lookup_table with items")
    {
        LookupTable1<unsigned int, string> lookup_table(971);

        const unsigned int item_count = 100000;
        for (unsigned int i = 0; i < item_count; ++i)
        {
            lookup_table.add_or_update_mapping(i, to_string(i));
        }

        THEN("The items can be retrieved from multiple threads")
        {
            vector<jthread> threads;

            auto check_items = [](const LookupTable1<unsigned int, string>& lookup_table, unsigned int min, unsigned int max)
            {
                for (unsigned int i = min; i < max; ++i)
                {
                    const auto test_result = to_string(i);
                    const auto result      = lookup_table.value_for(i);
                    CHECK(result == test_result);
                }
            };

            const unsigned int thread_count = 5;
            static_assert(item_count % thread_count == 0);

            for (unsigned int i = 0; i < thread_count; ++i)
            {
                const auto interval = item_count / thread_count;
                threads.emplace_back(check_items, std::ref(lookup_table), i * interval, (i + 1u) * interval);
            }
        }
    }
}
