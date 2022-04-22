#include <thirdparty/test.hpp>

#include <iris/ranges/stride_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("stride_view");

TEST_CASE("forward_range")
{
    static const auto input
        = std::forward_list { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto view = input | views::stride(3);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 6);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto view = input | views::stride(3);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr--, 3);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr--, 6);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 6);
    CHECK_EQ(*curr--, 9);
    CHECK_EQ(*curr++, 6);
    CHECK_EQ(*curr++, 9);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
    auto view = input | views::stride(3);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, 0);
    curr += 1;
    CHECK_EQ(*curr, 3);
    curr += 1;
    CHECK_EQ(*curr, 6);
    curr += 1;
    CHECK_EQ(*curr, 9);
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 4;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_SUITE_END();
