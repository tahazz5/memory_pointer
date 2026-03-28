#include <cassert>
#include <iostream>
#include <string>

#include "myoptional.hpp"

int main() {
    myoptional<int> empty;
    assert(!empty);
    assert(!empty.has_value());

    myoptional<int> value(42);
    assert(value.has_value());
    assert(*value == 42);
    assert(value.value() == 42);

    value.emplace(123);
    assert(value.has_value());
    assert(value.value() == 123);

    value.reset();
    assert(!value.has_value());

    myoptional<std::string> text("hello");
    assert(text.has_value());
    assert(text->size() == 5);
    assert(text.value() == "hello");

    myoptional<std::string> copy = text;
    assert(copy.has_value());
    assert(copy.value() == "hello");

    myoptional<std::string> moved = std::move(copy);
    assert(moved.has_value());
    assert(moved.value() == "hello");

    std::cout << "myoptional tests passed\n";
    return 0;
}
