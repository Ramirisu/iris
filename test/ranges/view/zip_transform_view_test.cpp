#include <thirdparty/test.hpp>

#include <iris/ranges/view/zip_transform_view.hpp>

#include <forward_list>
#include <list>
#include <string>

using namespace iris;

TEST_SUITE_BEGIN("zip_transform_view");

TEST_CASE("empty")
{
    auto view = views::zip_transform([]() { return std::string(); });
    static_assert(
        std::same_as<decltype(view), std::ranges::empty_view<std::string>>);
}

struct ref_max {
    template <typename T>
    constexpr T& operator()(T& lhs, T& rhs) const
    {
        return lhs > rhs ? lhs : rhs;
    }
};

TEST_CASE("forward_range: fn returns pr-value")
{
    static const auto input0 = std::forward_list { 0, 4, 2 };
    static const auto input1 = std::forward_list { 1, 3, 5 };
    auto view = views::zip_transform(std::plus(), input0, input1);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::forward_iterator_tag>);
}

TEST_CASE("forward_range")
{
    static const auto input0 = std::forward_list { 0, 4, 2 };
    static const auto input1 = std::forward_list { 1, 3, 5 };
    auto view = views::zip_transform(ref_max(), input0, input1);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::forward_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::forward_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(*curr++, 5);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range: fn returns pr-value")
{
    static const auto input0 = std::list { 0, 4, 2 };
    static const auto input1 = std::list { 1, 3, 5 };
    auto view = views::zip_transform(std::plus(), input0, input1);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::bidirectional_iterator_tag>);
}

TEST_CASE("bidirectional_range")
{
    static const auto input0 = std::list { 0, 4, 2 };
    static const auto input1 = std::list { 1, 3, 5 };
    auto view = views::zip_transform(ref_max(), input0, input1);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::bidirectional_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::bidirectional_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(*curr++, 5);
    CHECK_EQ(curr, std::ranges::end(view));
    curr--;
    curr--;
    curr--;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range: fn returns pr-value")
{
    static const int input0[] = { 0, 4, 2 };
    static const int input1[] = { 1, 3, 5 };
    auto view = views::zip_transform(std::plus(), input0, input1);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::input_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::random_access_iterator_tag>);
}

TEST_CASE("random_access_range")
{
    static const int input0[] = { 0, 4, 2 };
    static const int input1[] = { 1, 3, 5 };
    auto view = views::zip_transform(ref_max(), input0, input1);
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::random_access_iterator_tag>);
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
    CHECK_EQ(*curr, 1);
    CHECK_EQ(curr[2], 5);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK_EQ(*curr, 4);
    CHECK_EQ(curr[1], 5);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK_EQ(*curr, 5);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -3);
    CHECK_EQ(std::ranges::end(view) - curr, 0);
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(*curr, 1);
}

TEST_CASE("range size are not the same")
{
    static const int input0[] = { 0, 4, 2, 9 };
    static const int input1[] = { 1, 3, 5 };
    auto view = views::zip_transform(ref_max(), input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(*curr++, 5);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();
