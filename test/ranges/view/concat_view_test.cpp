#include <thirdparty/test.hpp>

#include <iris/ranges/view/concat_view.hpp>

#include <array>
#include <forward_list>
#include <list>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("concat_view");

TEST_CASE("result of applying range adaptor object")
{
    static_assert(
        std::same_as<decltype(views::concat(std::views::single(0))),
                     std::views::all_t<std::ranges::single_view<int>>>);
    static_assert(
        std::same_as<decltype(views::concat(std::views::single(0),
                                            std::views::single(0))),
                     ranges::concat_view<
                         std::views::all_t<std::ranges::single_view<int>>,
                         std::views::all_t<std::ranges::single_view<int>>>>);
}

TEST_CASE("forward_range")
{
    static const auto input0 = std::forward_list { 0, 1 };
    static const auto input1 = std::list { 2, 3, 4 };
    static const auto input2 = std::views::single(5);
    auto view = views::concat(input0, input1, input2);
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
    CHECK_EQ(*curr++, 5);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input0 = std::list { 0, 1 };
    static const auto input1 = std::array { 2, 3, 4 };
    static const auto input2 = std::views::single(5);
    auto view = views::concat(input0, input1, input2);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::bidirectional_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::bidirectional_iterator_tag>);
    CHECK_EQ(std::ranges::size(view), 6);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(*curr++, 5);
    CHECK_EQ(curr, std::ranges::end(view));
    --curr;
    CHECK_EQ(*curr--, 5);
    CHECK_EQ(*curr--, 4);
    CHECK_EQ(*curr--, 3);
    CHECK_EQ(*curr--, 2);
    CHECK_EQ(*curr--, 1);
    CHECK_EQ(*curr, 0);
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    static const auto input0 = std::vector { 0, 1 };
    static const auto input1 = std::array { 2, 3, 4 };
    static const auto input2 = std::views::single(5);
    auto view = views::concat(input0, input1, input2);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::random_access_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::random_access_iterator_tag>);
    CHECK_EQ(std::ranges::size(view), 6);
    CHECK_EQ(std::ranges::begin(view) + 6, std::ranges::end(view));
    CHECK_EQ(6 + std::ranges::begin(view), std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - 6, std::ranges::begin(view));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 6);
    CHECK_EQ(*curr, 0);
    CHECK_EQ(curr[1], 1);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 5);
    CHECK_EQ(*curr, 1);
    CHECK_EQ(curr[3], 4);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 4);
    CHECK_EQ(*curr, 2);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -3);
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    CHECK_EQ(*curr, 3);
    CHECK_EQ(curr[2], 5);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -4);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK_EQ(*curr, 4);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -5);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK_EQ(*curr, 5);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -6);
    CHECK_EQ(std::ranges::end(view) - curr, 0);
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 6;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_SUITE_END();
