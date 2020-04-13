#define CATCH_CONFIG_MAIN
#include "stack.hpp"
#include <catch2/catch.hpp>
#include <string>

using namespace algo;

TEST_CASE("push") {
    Stack<int> stk;

    stk.push(1);
    stk.push(2);
    stk.push(3);

    int i = 3;
    while (!stk.empty()) {
        REQUIRE(stk.top() == i);
        stk.pop();
        --i;
    }
}

TEST_CASE("emplace") {
    Stack<std::string> stk;

    stk.emplace("foo");
    stk.emplace("bar");

    REQUIRE(!stk.empty());
    REQUIRE(stk.top() == "bar");

    stk.pop();
    REQUIRE(stk.top() == "foo");

    stk.pop();
    REQUIRE(stk.empty());
}

TEST_CASE("swap") {
    Stack<int> stk, stk2;

    stk.push(1);
    stk.push(2);

    stk.swap(stk2);

    REQUIRE(stk.empty());

    REQUIRE(stk2.top() == 2);
    stk2.pop();

    REQUIRE(stk2.top() == 1);
    stk2.pop();

    REQUIRE(stk2.empty());
}
