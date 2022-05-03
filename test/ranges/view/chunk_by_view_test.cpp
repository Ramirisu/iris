#include <thirdparty/test.hpp>

#include <iris/ranges/view/chunk_by_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("chunk_by_view");

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list { 0, 1, 2, 1, 3, 6, 10, 5, 8 };
    auto view = input | views::chunk_by(std::ranges::less_equal {});
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::forward_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::forward_list { 0, 1, 2 }));
    CHECK(std::ranges::equal(*curr++, std::forward_list { 1, 3, 6, 10 }));
    CHECK(std::ranges::equal(*curr++, std::forward_list { 5, 8 }));
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1, 2, 1, 3, 6, 10, 5, 8 };
    auto view = input | views::chunk_by(std::ranges::less_equal {});
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::bidirectional_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::list { 0, 1, 2 }));
    CHECK(std::ranges::equal(*curr--, std::list { 1, 3, 6, 10 }));
    CHECK(std::ranges::equal(*curr++, std::list { 0, 1, 2 }));
    CHECK(std::ranges::equal(*curr++, std::list { 1, 3, 6, 10 }));
    CHECK(std::ranges::equal(*curr--, std::list { 5, 8 }));
    CHECK(std::ranges::equal(*curr++, std::list { 1, 3, 6, 10 }));
    CHECK(std::ranges::equal(*curr++, std::list { 5, 8 }));
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();
