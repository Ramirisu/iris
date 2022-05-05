#include <thirdparty/test.hpp>

#include <iris/ranges/view/chunk_view.hpp>

#include <forward_list>
#include <list>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("chunk_view");

TEST_CASE("input_range")
{
    auto input = std::istringstream("0 1 2 3 4");
    auto view = std::ranges::istream_view<std::string>(input) | views::chunk(2);
    using view_type = decltype(view);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::input_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr, std::vector { "0", "1" }));
    ++curr;
    CHECK(std::ranges::equal(*curr, std::vector { "2", "3" }));
    ++curr;
    CHECK(std::ranges::equal(*curr, std::vector { "4" }));
    ++curr;
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list { 0, 1, 2, 3, 4 };
    auto view = input | views::chunk(2);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::forward_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::forward_list { 0, 1 }));
    CHECK(std::ranges::equal(*curr++, std::forward_list { 2, 3 }));
    CHECK(std::ranges::equal(*curr++, std::forward_list { 4 }));
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1, 2, 3, 4 };
    auto view = input | views::chunk(2);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::bidirectional_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::list { 0, 1 }));
    CHECK(std::ranges::equal(*curr++, std::list { 2, 3 }));
    CHECK(std::ranges::equal(*curr++, std::list { 4 }));
    CHECK_EQ(curr, std::ranges::end(view));
    curr--;
    curr--;
    curr--;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = input | views::chunk(2);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::random_access_iterator_tag>);
    CHECK_EQ(std::ranges::size(view), 3);
    CHECK_EQ(std::ranges::begin(view) + 3, std::ranges::end(view));
    CHECK_EQ(3 + std::ranges::begin(view), std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - 3, std::ranges::begin(view));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    CHECK(std::ranges::equal(*curr, std::vector { 0, 1 }));
    CHECK(std::ranges::equal(curr[2], std::vector { 4 }));
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK(std::ranges::equal(*curr, std::vector { 2, 3 }));
    CHECK(std::ranges::equal(curr[1], std::vector { 4 }));
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK(std::ranges::equal(*curr, std::vector { 4 }));
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_SUITE_END();
