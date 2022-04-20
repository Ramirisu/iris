#include <thirdparty/test.hpp>

#include <iris/ranges/zip_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("zip_view");

TEST_CASE("value_type: tuple or pair")
{
    static_assert(std::same_as<decltype(views::zip()),
                               std::ranges::empty_view<std::tuple<>>>);
    static_assert(std::same_as<std::ranges::range_value_t<decltype(views::zip(
                                   std::views::iota(0, 1)))>,
                               std::tuple<int>>);
    static_assert(
        std::same_as<std::ranges::range_value_t<decltype(views::zip(
                         std::views::iota(0, 1), std::views::iota(0, 1)))>,
                     std::pair<int, int>>);
    static_assert(
        std::same_as<std::ranges::range_value_t<decltype(views::zip(
                         std::views::iota(0, 1), std::views::iota(0, 1),
                         std::views::iota(0, 1)))>,
                     std::tuple<int, int, int>>);
}

TEST_CASE("value_type: value or reference")
{
    static const int lvalue[] = { 0, 1, 2 };
    static_assert(
        std::same_as<std::ranges::range_value_t<decltype(views::zip(lvalue))>,
                     std::tuple<const int&>>);
    static_assert(std::same_as<std::ranges::range_value_t<decltype(views::zip(
                                   std::views::iota(0, 1)))>,
                               std::tuple<int>>);
}

TEST_CASE("forward_range")
{
    static const auto input0 = std::forward_list { 0, 1, 2 };
    static const auto input1 = std::forward_list { 0u, 1u, 2u };
    static const auto input2 = std::forward_list { '0', '1', '2' };
    auto view = views::zip(input0, input1, input2);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, 0u, '0' });
    CHECK_EQ(*curr++, std::tuple { 1, 1u, '1' });
    CHECK_EQ(*curr++, std::tuple { 2, 2u, '2' });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input0 = std::list { 0, 1, 2 };
    static const auto input1 = std::list { 0u, 1u, 2u };
    static const auto input2 = std::list { '0', '1', '2' };
    auto view = views::zip(input0, input1, input2);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, 0u, '0' });
    CHECK_EQ(*curr--, std::tuple { 1, 1u, '1' });
    CHECK_EQ(*curr++, std::tuple { 0, 0u, '0' });
    CHECK_EQ(*curr++, std::tuple { 1, 1u, '1' });
    CHECK_EQ(*curr--, std::tuple { 2, 2u, '2' });
    CHECK_EQ(*curr++, std::tuple { 1, 1u, '1' });
    CHECK_EQ(*curr++, std::tuple { 2, 2u, '2' });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("random_access_range")
{
    static const int input0[] = { 0, 1, 2 };
    static const std::uint32_t input1[] = { 0u, 1u, 2u };
    static const char input2[] = { '0', '1', '2' };
    auto view = views::zip(input0, input1, input2);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, std::tuple { 0, 0u, '0' });
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 1, 1u, '1' });
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 2, 2u, '2' });
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(*curr, std::tuple { 0, 0u, '0' });
}

TEST_CASE("range size are not the same")
{
    static const int input0[] = { 0, 1, 2, 3 };
    static const int input1[] = { 0u, 1u, 2u, 3u, 4u };
    static const char input2[] = { '0', '1', '2' };
    auto view = views::zip(input0, input1, input2);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, 0u, '0' });
    CHECK_EQ(*curr++, std::tuple { 1, 1u, '1' });
    CHECK_EQ(*curr++, std::tuple { 2, 2u, '2' });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();
