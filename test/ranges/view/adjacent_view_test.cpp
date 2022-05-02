#include <thirdparty/test.hpp>

#include <iris/ranges/view/adjacent_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("adjacent_view");

TEST_CASE("value_type: tuple or pair")
{
    static_assert(std::same_as<decltype(views::adjacent<0>()),
                               std::ranges::empty_view<std::tuple<>>>);
    static_assert(
        std::same_as<std::ranges::range_value_t<decltype(views::adjacent<1>(
                         std::views::iota(0, 1)))>,
                     std::tuple<int>>);
    static_assert(
        std::same_as<std::ranges::range_value_t<decltype(views::adjacent<2>(
                         std::views::iota(0, 1)))>,
                     std::pair<int, int>>);
    static_assert(
        std::same_as<std::ranges::range_value_t<decltype(views::pairwise(
                         std::views::iota(0, 1)))>,
                     std::pair<int, int>>);
    static_assert(
        std::same_as<std::ranges::range_value_t<decltype(views::adjacent<3>(
                         std::views::iota(0, 1)))>,
                     std::tuple<int, int, int>>);
}

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list { 0, 1, 2, 3, 4 };
    auto view = input | views::adjacent<3>;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, 1, 2 });
    CHECK_EQ(*curr++, std::tuple { 1, 2, 3 });
    CHECK_EQ(*curr++, std::tuple { 2, 3, 4 });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1, 2, 3, 4 };
    auto view = input | views::adjacent<3>;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, 1, 2 });
    CHECK_EQ(*curr++, std::tuple { 1, 2, 3 });
    CHECK_EQ(*curr++, std::tuple { 2, 3, 4 });
    CHECK_EQ(curr, std::ranges::end(view));
    curr--;
    curr--;
    curr--;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = input | views::adjacent<3>;
    CHECK_EQ(std::ranges::size(view), 3);
    CHECK_EQ(std::ranges::begin(view) + 3, std::ranges::end(view));
    CHECK_EQ(3 + std::ranges::begin(view), std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - 3, std::ranges::begin(view));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    CHECK_EQ(*curr, std::tuple { 0, 1, 2 });
    CHECK_EQ(curr[1], std::tuple { 1, 2, 3 });
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK_EQ(*curr, std::tuple { 1, 2, 3 });
    CHECK_EQ(curr[1], std::tuple { 2, 3, 4 });
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK_EQ(*curr, std::tuple { 2, 3, 4 });
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -3);
    CHECK_EQ(std::ranges::end(view) - curr, 0);
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_SUITE_END();
