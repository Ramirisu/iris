#include <thirdparty/test.hpp>

#include <iris/ranges/zip_transform_view.hpp>

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

TEST_CASE("forward_range")
{
    static const auto input0 = std::forward_list { 0, 1, 2 };
    static const auto input1 = std::forward_list { '0', '1', '2' };
    auto view = views::zip_transform(std::plus {}, input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 48);
    CHECK_EQ(*curr++, 50);
    CHECK_EQ(*curr++, 52);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input0 = std::list { 0, 1, 2 };
    static const auto input1 = std::list { '0', '1', '2' };
    auto view = views::zip_transform(std::plus {}, input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 48);
    CHECK_EQ(*curr--, 50);
    CHECK_EQ(*curr++, 48);
    CHECK_EQ(*curr++, 50);
    CHECK_EQ(*curr--, 52);
    CHECK_EQ(*curr++, 50);
    CHECK_EQ(*curr++, 52);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("random_access_range")
{
    static const int input0[] = { 0, 1, 2 };
    static const char input1[] = { '0', '1', '2' };
    auto view = views::zip_transform(std::plus {}, input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, 48);
    curr += 1;
    CHECK_EQ(*curr, 50);
    curr += 1;
    CHECK_EQ(*curr, 52);
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(*curr, 48);
}

TEST_CASE("range size are not the same")
{
    static const int input0[] = { 0, 1, 2, 3 };
    static const char input1[] = { '0', '1', '2' };
    auto view = views::zip_transform(std::plus {}, input0, input1);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 48);
    CHECK_EQ(*curr++, 50);
    CHECK_EQ(*curr++, 52);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_SUITE_END();