#include <thirdparty/test.hpp>

#include <iris/generator.hpp>

using namespace iris;

TEST_SUITE_BEGIN("generator");

static const std::string expected = "Hello World!";

generator<const std::string&> yield_lvalue_for_lvalue_generator()
{
    std::string lvalue;
    for (auto& c : expected) {
        lvalue = c;
        co_yield lvalue;
    }
}

generator<std::string> yield_rvalue_for_rvalue_generator()
{
    for (auto& c : expected) {
        co_yield std::string(1, c);
    }
}

generator<std::string> yield_lvalue_for_rvalue_generator()
{
    std::string lvalue;
    for (auto& c : expected) {
        lvalue = c;
        co_yield lvalue;
    }
}

TEST_CASE("co_yield lvalue for generator<const T&>")
{
    auto iter = std::ranges::begin(expected);
    for (auto&& value : yield_lvalue_for_lvalue_generator()) {
        CHECK_EQ(value, std::string(1, *iter++));
    }
    CHECK_EQ(iter, std::ranges::end(expected));
}

TEST_CASE("co_yield rvalue for generator<T&&>")
{
    auto iter = std::ranges::begin(expected);
    for (auto&& value : yield_rvalue_for_rvalue_generator()) {
        CHECK_EQ(value, std::string(1, *iter++));
    }
    CHECK_EQ(iter, std::ranges::end(expected));
}

TEST_CASE("co_yield lvalue for generator<T&&>")
{
    auto iter = std::ranges::begin(expected);
    for (auto&& value : yield_lvalue_for_rvalue_generator()) {
        CHECK_EQ(value, std::string(1, *iter++));
    }
    CHECK_EQ(iter, std::ranges::end(expected));
}

generator<const int&> iota(int start, int end)
{
    for (int i = start; i < end; ++i) {
        co_yield i;
    }
}

generator<const int&> nested(int start, int end)
{
    co_yield ranges::elements_of(iota(start, end));
}

TEST_CASE("co_yield elements_of(generator)")
{
    std::size_t count = 0;
    for (auto& value : nested(0, 10)) {
        CHECK_EQ(value, count++);
    }
    CHECK_EQ(count, 10);
}

generator<const int&> elements_of_range(int start, int end)
{
    co_yield ranges::elements_of(std::views::iota(start, end));
}

TEST_CASE("co_yield elements_of(range)")
{
    std::size_t count = 0;
    for (auto&& value : elements_of_range(0, 10)) {
        CHECK_EQ(value, count++);
    }
    CHECK_EQ(count, 10);
}

generator<const int&> deeply_nested()
{
    auto n1 = nested(0, 10);
    auto n2 = nested(10, 20);
    auto i1 = std::ranges::begin(n1);
    auto i2 = std::ranges::begin(n2);
    for (int i = 0; i < 10; ++i) {
        co_yield *i1;
        ++i1;
        co_yield *i2;
        ++i2;
    }
}

TEST_CASE("co_yield deeply nested generators")
{
    CHECK(std::ranges::equal(
        deeply_nested(),
        std::views::iota(0, 20) | std::views::transform([](auto value) {
            return (value / 2) + ((value % 2) == 1 ? 10 : 0);
        })));
}

TEST_SUITE_END();
