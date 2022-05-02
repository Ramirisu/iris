#include <thirdparty/test.hpp>

#include <iris/ranges/view/chunk_view.hpp>

#include <span>

using namespace iris;

TEST_SUITE_BEGIN("chunk_view");

TEST_CASE("input_range")
{
    using namespace std::string_literals;

    static const auto expected = std::vector<std::vector<std::string_view>> {
        { "0", "1" }, { "2", "3" }, { "4" }
    };
    auto input = std::istringstream("0 1 2 3 4");
    auto view = std::ranges::istream_view<std::string>(input) | views::chunk(2);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr, expected[0]));
    ++curr;
    CHECK(std::ranges::equal(*curr, expected[1]));
    ++curr;
    CHECK(std::ranges::equal(*curr, expected[2]));
    ++curr;
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("forward_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = input | views::chunk(2);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(0, 2)));
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(2, 2)));
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(4, 1)));
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = input | views::chunk(2);
    auto curr = std::ranges::begin(view);
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(0, 2)));
    CHECK(std::ranges::equal(*curr--, std::span(input).subspan(2, 2)));
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(0, 2)));
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(2, 2)));
    CHECK(std::ranges::equal(*curr--, std::span(input).subspan(4, 1)));
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(2, 2)));
    CHECK(std::ranges::equal(*curr++, std::span(input).subspan(4, 1)));
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = input | views::chunk(2);
    CHECK_EQ(std::ranges::size(view), 3);
    CHECK_EQ(std::ranges::begin(view) + 3, std::ranges::end(view));
    CHECK_EQ(3 + std::ranges::begin(view), std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - 3, std::ranges::begin(view));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    CHECK(std::ranges::equal(*curr, std::span(input).subspan(0, 2)));
    CHECK(std::ranges::equal(curr[2], std::span(input).subspan(4, 1)));
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK(std::ranges::equal(*curr, std::span(input).subspan(2, 2)));
    CHECK(std::ranges::equal(curr[1], std::span(input).subspan(4, 1)));
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK(std::ranges::equal(*curr, std::span(input).subspan(4, 1)));
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_SUITE_END();
