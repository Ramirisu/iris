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

generator<const int&> throws_exception()
{
    throw std::exception();
    co_yield 1;
}

TEST_CASE("generator body throws exception")
{
    auto run = []() {
        for (auto&& value : throws_exception()) {
            IRIS_UNUSED(value);
        }
    };
    CHECK_THROWS_AS(run(), std::exception);
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

generator<const int&> nested_throws_exception()
{
    co_yield ranges::elements_of(throws_exception());
}

TEST_CASE("nested generator body throws exception")
{
    auto run = []() {
        for (auto&& value : nested_throws_exception()) {
            IRIS_UNUSED(value);
        }
    };
    CHECK_THROWS_AS(run(), std::exception);
}

template <typename T>
class my_allocator {
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::false_type;

    T* allocate(size_type n)
    {
        return static_cast<T*>(std::malloc(n * sizeof(T)));
    }

    void deallocate(T* pointer, size_type n)
    {
        IRIS_UNUSED(n);
        std::free(pointer);
    }
};

TEST_CASE("operator new(std::size_t) with custom allocator")
{
    auto g = []() -> generator<const int&, int, my_allocator<int>> {
        for (int i = 0; i < 10; ++i) {
            co_yield i;
        }
    };

    int count = 0;
    for (auto&& value : g()) {
        CHECK_EQ(count++, value);
    }
    CHECK_EQ(count, 10);
}

TEST_SUITE_END();
