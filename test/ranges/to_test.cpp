#include <thirdparty/test.hpp>

#include <iris/ranges/to.hpp>

#include <list>

using namespace iris;

TEST_SUITE_BEGIN("ranges/to");

TEST_CASE("directly constructible")
{
    using from_type = std::vector<std::int32_t>;
    using to_type = std::vector<std::int64_t>;
    auto to = ranges::to<to_type>(from_type { 0, 1, 2, 3, 4 });
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to, to_type { 0, 1, 2, 3, 4 }));
}

template <typename T>
class range_constructible_only {
public:
    range_constructible_only(std::initializer_list<T> list)
        : data_(list)
    {
    }

    template <typename Iterator, typename Sentinel>
    range_constructible_only(Iterator first, Sentinel last)
        : data_(first, last)
    {
    }

    auto begin()
    {
        return data_.begin();
    }

    auto end()
    {
        return data_.end();
    }

private:
    std::vector<T> data_;
};

TEST_CASE("range constructible")
{
    using to_type = range_constructible_only<int>;
    auto to = ranges::to<to_type>(std::views::iota(0, 5));
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to, to_type { 0, 1, 2, 3, 4 }));
}

TEST_CASE("insertable")
{
    using to_type = std::vector<int>;
    auto to = ranges::to<to_type>(std::views::iota(0, 5));
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to, to_type { 0, 1, 2, 3, 4 }));
}

TEST_CASE("range of range")
{
    using to_type = std::vector<std::vector<int>>;
    auto to = ranges::to<to_type>(
        std::views::iota(0, 5)
        | std::views::transform([](int i) { return std::list<int>(i, i); }));
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to,
                             to_type {
                                 {},
                                 { 1 },
                                 { 2, 2 },
                                 { 3, 3, 3 },
                                 { 4, 4, 4, 4 },
                             }));
}

TEST_CASE("pipe")
{
    using to_type = std::vector<int>;
    auto to = std::views::iota(0, 5) | ranges::to<to_type>();
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to, to_type { 0, 1, 2, 3, 4 }));
}

TEST_CASE("[auto deduce] directly constructible")
{
    using to_type = std::vector<int>;
    auto to = ranges::to<std::vector>(std::vector<int> { 0, 1, 2, 3, 4 });
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to, std::vector<int> { 0, 1, 2, 3, 4 }));
}

TEST_CASE("[auto deduce] range constructible")
{
    using to_type = std::vector<int>;
    auto to = ranges::to<std::vector>(std::views::iota(0, 5));
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to, std::vector<int> { 0, 1, 2, 3, 4 }));
}

TEST_CASE("[auto deduce] pipe")
{
    using to_type = std::vector<int>;
    auto to = std::views::iota(0, 5) | ranges::to<std::vector>();
    static_assert(std::is_same_v<decltype(to), to_type>);
    CHECK(std::ranges::equal(to, to_type { 0, 1, 2, 3, 4 }));
}

TEST_SUITE_END();
