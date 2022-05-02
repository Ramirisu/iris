#include <thirdparty/test.hpp>

#include <iris/ranges/view/enumerate_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("enumerate_view");

namespace iris::ranges {
template <typename Index, typename Value>
constexpr bool operator==(const enumerate_result<Index, Value>& lhs,
                          const enumerate_result<Index, Value>& rhs)
{
    return lhs.index == rhs.index && lhs.value == rhs.value;
}
}

TEST_CASE("forward_range")
{
    using type = ranges::enumerate_result<std::size_t, const int&>;
    static const auto input = std::forward_list { 100, 101, 102 };
    auto view = input | views::enumerate;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, type { 0, 100 });
    CHECK_EQ(*curr++, type { 1, 101 });
    CHECK_EQ(*curr++, type { 2, 102 });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    using type = ranges::enumerate_result<std::size_t, const int&>;
    static const auto input = std::list { 100, 101, 102 };
    auto view = input | views::enumerate;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, type { 0, 100 });
    CHECK_EQ(*curr++, type { 1, 101 });
    CHECK_EQ(*curr++, type { 2, 102 });
    CHECK_EQ(curr, std::ranges::end(view));
    curr--;
    curr--;
    curr--;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    using type = ranges::enumerate_result<std::size_t, const int&>;
    static const int input[] = { 100, 101, 102 };
    auto view = input | views::enumerate;
    CHECK_EQ(std::ranges::size(view), 3);
    CHECK_EQ(std::ranges::begin(view) + 3, std::ranges::end(view));
    CHECK_EQ(3 + std::ranges::begin(view), std::ranges::end(view));
    CHECK_EQ(std::ranges::end(view) - 3, std::ranges::begin(view));
    auto curr = std::ranges::begin(view);
    CHECK_EQ(std::ranges::begin(view) - curr, 0);
    CHECK_EQ(std::ranges::end(view) - curr, 3);
    CHECK_EQ(*curr, type { 0, 100 });
    CHECK_EQ(curr[1], type { 1, 101 });
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -1);
    CHECK_EQ(std::ranges::end(view) - curr, 2);
    CHECK_EQ(*curr, type { 1, 101 });
    CHECK_EQ(curr[1], type { 2, 102 });
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -2);
    CHECK_EQ(std::ranges::end(view) - curr, 1);
    CHECK_EQ(*curr, type { 2, 102 });
    curr += 1;
    CHECK_EQ(std::ranges::begin(view) - curr, -3);
    CHECK_EQ(std::ranges::end(view) - curr, 0);
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 3;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("index & value")
{
    using type = ranges::enumerate_result<std::size_t, const int&>;
    static const int input[] = { 1, 2, 4, 8 };
    auto view = input | views::enumerate;
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, type { 0, input[0] });
    CHECK_EQ(*curr++, type { 1, input[1] });
    CHECK_EQ(*curr++, type { 2, input[2] });
    CHECK_EQ(*curr++, type { 3, input[3] });
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("for loop")
{
    static const int input[] = { 1, 2, 4, 8 };
    std::size_t i = 0;

    SUBCASE("enumerate_result")
    {
        for (auto&& element : input | views::enumerate) {
            static_assert(!std::is_reference_v<decltype(element.index)>);
            static_assert(std::is_lvalue_reference_v<decltype(element.value)>);
            CHECK_EQ(element.index, i);
            CHECK_EQ(element.value, input[i]);
            static_assert(
                std::is_lvalue_reference_v<decltype(std::get<0>(element))>);
            static_assert(
                std::is_lvalue_reference_v<decltype(std::get<1>(element))>);
            CHECK_EQ(std::get<0>(element), i);
            CHECK_EQ(std::get<1>(element), input[i]);
            ++i;
        }
    }

    SUBCASE("structured binding")
    {
        for (auto&& [index, value] : input | views::enumerate) {
            static_assert(!std::is_reference_v<decltype(index)>);
            static_assert(std::is_lvalue_reference_v<decltype(value)>);
            CHECK_EQ(index, i);
            CHECK_EQ(value, input[i]);
            ++i;
        }
    }
}

TEST_SUITE_END();
