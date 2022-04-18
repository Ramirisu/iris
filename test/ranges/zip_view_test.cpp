#include <thirdparty/test.hpp>

#include <iris/ranges/zip_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("zip_view");

TEST_CASE("empty")
{
    auto view = views::zip();
    static_assert(
        std::same_as<decltype(view), std::ranges::empty_view<std::tuple<>>>);
}

TEST_CASE("forward_range")
{
    static const auto input0 = std::forward_list { 0, 1, 2 };
    static const auto input1 = std::forward_list { '0', '1', '2' };
    auto view = views::zip(input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, '0' });
    CHECK_EQ(*curr++, std::tuple { 1, '1' });
    CHECK_EQ(*curr++, std::tuple { 2, '2' });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input0 = std::list { 0, 1, 2 };
    static const auto input1 = std::list { '0', '1', '2' };
    auto view = views::zip(input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, '0' });
    CHECK_EQ(*curr--, std::tuple { 1, '1' });
    CHECK_EQ(*curr++, std::tuple { 0, '0' });
    CHECK_EQ(*curr++, std::tuple { 1, '1' });
    CHECK_EQ(*curr--, std::tuple { 2, '2' });
    CHECK_EQ(*curr++, std::tuple { 1, '1' });
    CHECK_EQ(*curr++, std::tuple { 2, '2' });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("random_access_range")
{
    static const int input0[] = { 0, 1, 2 };
    static const char input1[] = { '0', '1', '2' };
    auto view = views::zip(input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, std::tuple { 0, '0' });
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 1, '1' });
    curr += 1;
    CHECK_EQ(*curr, std::tuple { 2, '2' });
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(*curr, std::tuple { 0, '0' });
}

TEST_CASE("range size are not the same")
{
    static const int input0[] = { 0, 1, 2, 3 };
    static const char input1[] = { '0', '1', '2' };
    auto view = views::zip(input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, std::tuple { 0, '0' });
    CHECK_EQ(*curr++, std::tuple { 1, '1' });
    CHECK_EQ(*curr++, std::tuple { 2, '2' });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();
