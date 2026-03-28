#include <cassert>
#include <iostream>
#include <string>

#include "myvariant.hpp"

int main() {
    myvariant<int, std::string> value(10);
    assert(value.holds_alternative<int>());
    assert(value.get<int>() == 10);

    value = std::string("hello");
    assert(value.holds_alternative<std::string>());
    assert(value.get<std::string>() == "hello");

    myvariant<int, std::string> copy = value;
    assert(copy.holds_alternative<std::string>());
    assert(copy.get<std::string>() == "hello");

    myvariant<int, std::string> moved = std::move(value);
    assert(moved.holds_alternative<std::string>());
    assert(moved.get<std::string>() == "hello");

    value = 123;
    assert(value.holds_alternative<int>());
    assert(value.get<int>() == 123);

    std::cout << "myvariant tests passed\n";
    return 0;
}
