#include <thirdparty/test.hpp>

#include <iris/algorithm/fold.hpp>

#include <algorithm>

using namespace iris;

TEST_SUITE_BEGIN("fold");

TEST_CASE("fold_left")
{
    static const std::string input[] = { "1", "2", "3", "4" };
    CHECK_EQ(ranges::fold_left(std::ranges::begin(input),
                               std::ranges::end(input), std::string("0"),
                               std::plus {}),
             std::string_view("01234"));
    CHECK_EQ(ranges::fold_left(input, std::string("0"), std::plus {}),
             std::string_view("01234"));
}

TEST_CASE("fold_left_first")
{
    static const std::string input[] = { "1", "2", "3", "4" };
    CHECK_EQ(ranges::fold_left_first(std::ranges::begin(input),
                                     std::ranges::end(input), std::plus {}),
             std::string_view("1234"));
    CHECK_EQ(ranges::fold_left_first(input, std::plus {}),
             std::string_view("1234"));

    static const auto empty = std::views::empty<int>;
    CHECK(!ranges::fold_left_first(empty, std::plus {}));
}

TEST_CASE("fold_right")
{
    static const std::string input[] = { "1", "2", "3", "4" };
    CHECK_EQ(ranges::fold_right(std::ranges::begin(input),
                                std::ranges::end(input), std::string("0"),
                                std::plus {}),
             std::string_view("12340"));
    CHECK_EQ(ranges::fold_right(input, std::string("0"), std::plus {}),
             std::string_view("12340"));
}

TEST_CASE("fold_right_last")
{
    static const std::string input[] = { "1", "2", "3", "4" };
    CHECK_EQ(ranges::fold_right_last(std::ranges::begin(input),
                                     std::ranges::end(input), std::plus {}),
             std::string_view("1234"));
    CHECK_EQ(ranges::fold_right_last(input, std::plus {}),
             std::string_view("1234"));

    static const auto empty = std::views::empty<int>;
    CHECK(!ranges::fold_right_last(empty, std::plus {}));
}

TEST_SUITE_END();
