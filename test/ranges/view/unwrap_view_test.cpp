#include <thirdparty/test.hpp>

#include <iris/expected.hpp>
#include <iris/ranges/view/unwrap_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("unwrap_view");

TEST_CASE("forward_range")
{
    static const auto input
        = std::forward_list<expected<int, int>> { 0, 1, 2, 3, 4 };
    auto view = input | views::unwrap;
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::forward_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::forward_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list<expected<int, int>> { 0, 1, 2, 3, 4 };
    auto view = input | views::unwrap;
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::bidirectional_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::bidirectional_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
    curr--;
    CHECK_EQ(*curr--, 4);
    CHECK_EQ(*curr--, 3);
    CHECK_EQ(*curr--, 2);
    CHECK_EQ(*curr--, 1);
    CHECK_EQ(*curr, 0);
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    static const expected<int, int> input[] { 0, 1, 2, 3, 4 };
    auto view = input | views::unwrap;
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::random_access_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::random_access_iterator_tag>);
    CHECK_EQ(std::ranges::size(view), 5);
    CHECK_EQ(std::ranges::begin(view) + 5, std::ranges::end(view));
    CHECK_EQ(5 + std::ranges::begin(view), std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - 5, std::ranges::begin(view));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 5);
    CHECK_EQ(*curr, 0);
    CHECK_EQ(curr[2], 2);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 4);
    CHECK_EQ(*curr, 1);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    CHECK_EQ(*curr, 2);
    CHECK_EQ(curr[2], 4);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -3);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK_EQ(*curr, 3);
    CHECK_EQ(curr[1], 4);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -4);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK_EQ(*curr, 4);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -5);
    CHECK_EQ(std::ranges::end(view) - curr, 0);
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 5;
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 5);
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("expected")
{
    static const expected<int, int> input[] { 0, 1, 2, 3, 4 };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("expected with error")
{
    static const expected<int, int> input[] { 0, 1, 2, 3, iris::unexpected(4) };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_THROWS_AS(IRIS_UNUSED(*curr++), bad_expected_access<int>);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("optional")
{
    static const std::optional<int> input[] { 0, 1, 2, 3, 4 };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("optional with error")
{
    static const std::optional<int> input[] { 0, 1, 2, 3, std::nullopt };
    auto view = input | views::unwrap;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_THROWS_AS(IRIS_UNUSED(*curr++), std::bad_optional_access);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();
