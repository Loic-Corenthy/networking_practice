#include "TSLockFreeStack.hpp"

#include <catch2/catch_test_macros.hpp>

using LCNS::ThreadSafe::LockFreeStack;

TEST_CASE("Basic", "[test]")
{
    LockFreeStack<int> stack;

    for (int i = 0; i < 10; i++)
    {
        stack.push(i);
    }

    for (int i = 10 - 1; i >= 0; i--)
    {
        CHECK(*stack.pop() == i);
    }
}