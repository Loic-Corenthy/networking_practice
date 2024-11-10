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
        auto my_const_print = [](const int& i) { std::cout << i << '\n'; };
        auto multiply_by_two = [](int& i) { i *= 2; };

        list.push_front(4);
        list.push_front(5);
        list.push_front(6);

        list.for_each(my_print);
        list.for_each(multiply_by_two);
        list.for_each(my_const_print);

        auto divisible_by_2 = [](int i ) { return i % 2 == 0;};
        auto res = list.find_first_if(divisible_by_2);
        CHECK(*res == 12);

        auto more_than_20 = [](int i ) { return i > 20;};
        res = list.find_first_if(more_than_20);
        CHECK(!res);
    }
}