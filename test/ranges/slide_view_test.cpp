#include <thirdparty/test.hpp>

#include <iris/ranges/slide_view.hpp>

#include <forward_list>
#include <list>
#include <span>

using namespace iris;

TEST_SUITE_BEGIN("slide_view");

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list { 0, 1, 2, 3, 4 };
    auto view = input | views::slide(2);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::forward_list { 0, 1 }));
    CHECK(std::ranges::equal(*curr++, std::forward_list { 1, 2 }));
    CHECK(std::ranges::equal(*curr++, std::forward_list { 2, 3 }));
    CHECK(std::ranges::equal(*curr++, std::forward_list { 3, 4 }));
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1, 2, 3, 4 };
    auto view = input | views::slide(2);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::list { 0, 1 }));
    CHECK(std::ranges::equal(*curr++, std::list { 1, 2 }));
    CHECK(std::ranges::equal(*curr--, std::list { 2, 3 }));
    CHECK(std::ranges::equal(*curr++, std::list { 1, 2 }));
    CHECK(std::ranges::equal(*curr++, std::list { 2, 3 }));
    CHECK(std::ranges::equal(*curr--, std::list { 3, 4 }));
    CHECK(std::ranges::equal(*curr++, std::list { 2, 3 }));
    CHECK(std::ranges::equal(*curr++, std::list { 3, 4 }));
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = input | views::slide(2);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr, std::span(input).subspan(0, 2)));
    curr += 1;
    CHECK(std::ranges::equal(*curr, std::span(input).subspan(1, 2)));
    curr += 1;
    CHECK(std::ranges::equal(*curr, std::span(input).subspan(2, 2)));
    curr += 1;
    CHECK(std::ranges::equal(*curr, std::span(input).subspan(3, 2)));
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 4;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_SUITE_END();
