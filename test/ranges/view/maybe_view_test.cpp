#include <thirdparty/test.hpp>

#include <iris/ranges/view/maybe_view.hpp>

#include <optional>

using namespace iris;

TEST_SUITE_BEGIN("maybe_view");

TEST_CASE("optional: value")
{
    static const auto input = std::optional { 4 };
    auto view = input | views::maybe;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("optional: nullopt")
{
    static const auto input = std::optional<int> {};
    auto view = input | views::maybe;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("optional: using ref to modify underlying nullable")
{
    auto input = std::optional { 4 };
    auto view = std::ref(input) | views::maybe;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, 4);
    *curr++ = 10;
    CHECK_EQ(curr, std::ranges::end(view));
    CHECK_EQ(*input, 10);
}

TEST_CASE("ranges of nullable")
{
    static const auto input
        = std::vector { std::optional<int> { 42 }, std::optional<int> {},
                        std::optional<int> { 6 * 9 } };
    auto view = std::views::join(std::views::transform(input, views::maybe));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, 42);
    ++curr;
    CHECK_EQ(*curr, 54);
    ++curr;
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();
