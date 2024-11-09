#include "TSList1.hpp"

#include "Helper.hpp"

#include <catch2/catch_test_macros.hpp>

#include <thread>

using LCNS::ThreadSafe::List1;

bool f([[maybe_unused]] int i)
{
    return true;
}

TEST_CASE("Empty list", "[test][internal]")
{
    GIVEN("An empty list")
    {
        List1<int> list;

        auto predicate = []([[maybe_unused]] int i) -> bool { return true; };

        list.remove_if<decltype(predicate)>(predicate);
    }
}