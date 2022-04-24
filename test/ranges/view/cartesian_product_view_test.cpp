#include <thirdparty/test.hpp>

#include <iris/ranges/view/cartesian_product_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("cartesian_product_view");

TEST_CASE("value_type: tuple or pair")
{
    static_assert(std::same_as<decltype(views::cartesian_product()),
                               std::ranges::empty_view<std::tuple<>>>);
    static_assert(std::same_as<
                  std::ranges::range_value_t<decltype(views::cartesian_product(
                      std::views::iota(0, 1)))>,
                  std::tuple<int>>);
    static_assert(std::same_as<
                  std::ranges::range_value_t<decltype(views::cartesian_product(
                      std::views::iota(0, 1), std::views::iota(0, 1)))>,
                  std::pair<int, int>>);
    static_assert(std::same_as<
                  std::ranges::range_value_t<decltype(views::cartesian_product(
                      std::views::iota(0, 1), std::views::iota(0, 1),
                      std::views::iota(0, 1)))>,
                  std::tuple<int, int, int>>);
}

TEST_CASE("forward_range")
{
    static const auto input0 = std::forward_list { 0, 1 };
    static const auto input1 = std::forward_list { 2, 3 };
    static const auto input2 = std::forward_list { 4, 5 };
    auto view = views::cartesian_product(input0, input1, input2);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, 2, 4 });
    CHECK_EQ(*curr++, std::tuple { 0, 2, 5 });
    CHECK_EQ(*curr++, std::tuple { 0, 3, 4 });
    CHECK_EQ(*curr++, std::tuple { 0, 3, 5 });
    CHECK_EQ(*curr++, std::tuple { 1, 2, 4 });
    CHECK_EQ(*curr++, std::tuple { 1, 2, 5 });
    CHECK_EQ(*curr++, std::tuple { 1, 3, 4 });
    CHECK_EQ(*curr++, std::tuple { 1, 3, 5 });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input0 = std::list { 0, 1 };
    static const auto input1 = std::list { 2, 3 };
    static const auto input2 = std::list { 4, 5 };
    auto view = views::cartesian_product(input0, input1, input2);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, 2, 4 });
    CHECK_EQ(*curr++, std::tuple { 0, 2, 5 });
    CHECK_EQ(*curr++, std::tuple { 0, 3, 4 });
    CHECK_EQ(*curr++, std::tuple { 0, 3, 5 });
    CHECK_EQ(*curr++, std::tuple { 1, 2, 4 });
    CHECK_EQ(*curr++, std::tuple { 1, 2, 5 });
    CHECK_EQ(*curr++, std::tuple { 1, 3, 4 });
    CHECK_EQ(*curr++, std::tuple { 1, 3, 5 });
    CHECK_EQ(curr, std::ranges::end(view));
    CHECK_EQ(*--curr, std::tuple { 1, 3, 5 });
    CHECK_EQ(*--curr, std::tuple { 1, 3, 4 });
    CHECK_EQ(*--curr, std::tuple { 1, 2, 5 });
    CHECK_EQ(*--curr, std::tuple { 1, 2, 4 });
    CHECK_EQ(*--curr, std::tuple { 0, 3, 5 });
    CHECK_EQ(*--curr, std::tuple { 0, 3, 4 });
    CHECK_EQ(*--curr, std::tuple { 0, 2, 5 });
    CHECK_EQ(*--curr, std::tuple { 0, 2, 4 });
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    static const int input0[] = { 0, 1 };
    static const int input1[] = { 2, 3 };
    static const int input2[] = { 4, 5 };
    auto view = views::cartesian_product(input0, input1, input2);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, std::tuple { 0, 2, 4 });
    CHECK_EQ(std::ranges::end(view) - curr, 8);
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 0, 2, 5 });
    CHECK_EQ(std::ranges::end(view) - curr, 7);
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 0, 3, 4 });
    CHECK_EQ(std::ranges::end(view) - curr, 6);
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 0, 3, 5 });
    CHECK_EQ(std::ranges::end(view) - curr, 5);
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 1, 2, 4 });
    CHECK_EQ(std::ranges::end(view) - curr, 4);
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 1, 2, 5 });
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 1, 3, 4 });
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 1, 3, 5 });
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - curr, 0);
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 1, 3, 5 });
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 1, 3, 4 });
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 1, 2, 5 });
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 1, 2, 4 });
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 0, 3, 5 });
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 0, 3, 4 });
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 0, 2, 5 });
    curr -= 1;
    CHECK_EQ(*curr, std::tuple { 0, 2, 4 });
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_SUITE_END();
