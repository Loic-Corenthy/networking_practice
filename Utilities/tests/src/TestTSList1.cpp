#include "TSList1.hpp"

#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>
#include <iostream>

using LCNS::ThreadSafe::List1;

TEST_CASE("Empty list", "[test][internal]")
{
    GIVEN("An empty list")
    {
        List1<int> list;

        list.push_front(1);
        list.push_front(2);
        list.push_front(3);

        auto predicate = [](int i) { return i % 2 == 0 || i % 2 == 1; };

        list.remove_if(predicate);

        auto my_print = [](int i) { std::cout << i << '\n'; };
        auto multiply_by_two = [](int& i) { i *= 2; };

        list.push_front(4);
        list.push_front(5);
        list.push_front(6);

        list.for_each(my_print);
        list.for_each(multiply_by_two);
        list.for_each(my_print);

        CHECK(true);
    }
}