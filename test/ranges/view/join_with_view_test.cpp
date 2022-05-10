#include <thirdparty/test.hpp>

#include <iris/ranges/view/join_with_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("join_with_view");

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list<std::forward_list<int>> {
        { 0, 1 }, { 2 }, {}, { 3, 4, 5 }
    };
    static const auto pattern = std::forward_list { 8, 9 };
    auto view = input | views::join_with(pattern);
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
    CHECK_EQ(*curr++, 8);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 8);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(*curr++, 8);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(*curr++, 5);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input
        = std::list<std::list<int>> { { 0, 1 }, { 2 }, {}, { 3, 4, 5 } };
    static const auto pattern = std::list { 8, 9 };
    auto view = input | views::join_with(pattern);
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
    CHECK_EQ(*curr++, 8);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 8);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(*curr++, 8);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(*curr++, 5);
    CHECK_EQ(curr, std::ranges::end(view));
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("glvalue inner range")
{
    static const auto expected = std::string_view("the-quick-brown-fox");
    static const auto pattern = std::string_view("-");
    std::vector<std::string> range = { "the", "quick", "brown", "fox" };
    CHECK(std::ranges::equal(range | views::join_with(pattern), expected));
}

TEST_CASE("non-glvalue inner range")
{
    static const auto expected = std::string_view("the-quick-brown-fox");
    static const auto pattern = std::string_view("-");
    CHECK(std::ranges::equal(expected | std::views::split(pattern)
                                 | views::join_with(pattern),
                             expected));
}

TEST_SUITE_END();
