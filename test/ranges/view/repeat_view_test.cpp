#include <thirdparty/test.hpp>

#include <iris/ranges/view/repeat_view.hpp>

#include <forward_list>
#include <list>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("repeat_view");

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list { 0, 1 };
    auto view = views::repeat(input, 3);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::forward_list { 0, 1 });
    CHECK_EQ(*curr++, std::forward_list { 0, 1 });
    CHECK_EQ(*curr++, std::forward_list { 0, 1 });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1 };
    auto view = views::repeat(input, 3);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::list { 0, 1 });
    CHECK_EQ(*curr++, std::list { 0, 1 });
    CHECK_EQ(*curr++, std::list { 0, 1 });
    CHECK_EQ(curr, std::ranges::end(view));
    --curr;
    CHECK_EQ(*curr--, std::list { 0, 1 });
    CHECK_EQ(*curr--, std::list { 0, 1 });
    CHECK_EQ(*curr, std::list { 0, 1 });
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1 };
    auto view = views::repeat(input, 3);
    CHECK_EQ(std::ranges::size(view), 3);
    CHECK_EQ(std::ranges::begin(view) + 3, std::ranges::end(view));
    CHECK_EQ(3 + std::ranges::begin(view), std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - 3, std::ranges::begin(view));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    CHECK_EQ(*curr, input);
    CHECK_EQ(curr[2], input);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK_EQ(*curr, input);
    CHECK_EQ(curr[1], input);
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK_EQ(*curr, input);
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("repeat_view")
{
    CHECK(std::ranges::equal(views::repeat(0, 5), std::vector<int>(5, 0)));
    CHECK(std::ranges::equal(views::repeat(0) | std::views::take(5),
                             std::vector<int>(5, 0)));
    CHECK(std::ranges::equal(views::repeat(std::views::iota(0, 5), 5)
                                 | std::views::join,
                             std::vector { 0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4, //
                                           0, 1, 2, 3, 4 }));
}

TEST_SUITE_END();
