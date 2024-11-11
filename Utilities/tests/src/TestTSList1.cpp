#include "TSList1.hpp"

#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators_random.hpp>

#include <thread>
#include <iostream>

using Catch::Generators::random;
using LCNS::ThreadSafe::List1;

class TestList1Fixture
{
public:
    static bool always_match([[maybe_unused]] int i)
    {
        return true;
    }

    static bool match_one(const int i)
    {
        return i == 1;
    };

    static bool match_two(const int i)
    {
        return i == 2;
    };

    static bool match_three(const int i)
    {
        return i == 3;
    };
};

TEST_CASE_METHOD(TestList1Fixture, "Empty list", "[test][internal]")
{
    GIVEN("An empty list")
    {
        List1<int> list;

        WHEN("Trying to get any element")
        {
            const auto result = list.find_first_if(TestList1Fixture::always_match);

            THEN("The list returns nullptr")
            {
                CHECK(result == nullptr);
            }
        }
    }

    GIVEN("A list with added elements")
    {
        List1<int> list;

        const auto count = random(10, 100).get();
        for (int i = 0; i < count; ++i)
        {
            list.push_front(random(-1000, 1000).get());
        }

        WHEN("All the elements are removed")
        {
            for (int i = 0; i < count; ++i)
            {
                list.remove_if(TestList1Fixture::always_match);
            }

            THEN("The list returns nullptr")
            {
                const auto result = list.find_first_if(TestList1Fixture::always_match);
                CHECK(result == nullptr);
            }
        }
    }
}

TEST_CASE_METHOD(TestList1Fixture, "Adding elements", "[test][internal]")
{
    GIVEN("A list with known added elements")
    {
        List1<int> list;

        list.push_front(1);
        list.push_front(2);
        list.push_front(3);

        WHEN("They are retrieved")
        {
            const auto one   = list.find_first_if(TestList1Fixture::match_one);
            const auto two   = list.find_first_if(TestList1Fixture::match_two);
            const auto three = list.find_first_if(TestList1Fixture::match_three);

            THEN("They match the original values")
            {
                CHECK(*one == 1);
                CHECK(*two == 2);
                CHECK(*three == 3);
            }
        }

        WHEN("Looking for a value not in the list")
        {
            const auto match_four = [](const int i) { return i == 4; };
            const auto four       = list.find_first_if(match_four);

            THEN("The list returns nullptr")
            {
                CHECK(four == nullptr);
            }
        }
    }
}

TEST_CASE_METHOD(TestList1Fixture, "Modifying elements", "[test][internal]")
{
    GIVEN("A list with known added elements")
    {
        List1<int> list;

        list.push_front(1);
        list.push_front(2);
        list.push_front(3);

        WHEN("They are modified")
        {
            const auto multiply_by_ten = [](int& i) { i *= 10; };
            list.for_each(multiply_by_ten);

            THEN("They match the updated values")
            {
                const auto match_ten    = [](const int i) { return i == 10; };
                const auto match_twenty = [](const int i) { return i == 20; };
                const auto match_thirty = [](const int i) { return i == 30; };

                const auto ten    = *list.find_first_if(match_ten);
                const auto twenty = *list.find_first_if(match_twenty);
                const auto thirty = *list.find_first_if(match_thirty);
                CHECK(ten == 10);
                CHECK(twenty == 20);
                CHECK(thirty == 30);
            }

            AND_WHEN("We look for the original values")
            {
                const auto one   = list.find_first_if(TestList1Fixture::match_one);
                const auto two   = list.find_first_if(TestList1Fixture::match_two);
                const auto three = list.find_first_if(TestList1Fixture::match_three);

                AND_THEN("We can't find them anymore")
                {
                    CHECK(one == nullptr);
                    CHECK(two == nullptr);
                    CHECK(three == nullptr);
                }
            }
        }
    }
}

TEST_CASE_METHOD(TestList1Fixture, "Removing elements", "[test][internal]")
{
    GIVEN("A list with known added elements")
    {
        List1<int> list;

        list.push_front(1);
        list.push_front(2);
        list.push_front(3);

        WHEN("We remove one of them")
        {
            list.remove_if(TestList1Fixture::match_two);

            THEN("It cannot be found but the other ones still can")
            {
                const auto one   = list.find_first_if(TestList1Fixture::match_one);
                const auto two   = list.find_first_if(TestList1Fixture::match_two);
                const auto three = list.find_first_if(TestList1Fixture::match_three);

                CHECK(*one == 1);
                CHECK(two == nullptr);
                CHECK(*three == 3);
            }
        }
    }
}