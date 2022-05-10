#include <thirdparty/test.hpp>

#include <iris/ranges/view/as_rvalue_view.hpp>

#include <forward_list>
#include <list>

using namespace iris;

TEST_SUITE_BEGIN("as_rvalue_view");

TEST_CASE("value_type")
{
    using view_type
        = decltype(views::as_rvalue(std::declval<std::vector<int>&>()));
    static_assert(std::same_as<std::ranges::range_value_t<view_type>, int>);
    static_assert(
        std::same_as<std::ranges::range_reference_t<view_type>, int&&>);
}

TEST_CASE("forward_range")
{
    static const auto input = std::forward_list { 0, 1, 2, 3, 4 };
    auto view = input | views::as_rvalue;
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::forward_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::input_iterator_tag>);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
}

TEST_CASE("bidirectional_range")
{
    static const auto input = std::list { 0, 1, 2, 3, 4 };
    auto view = input | views::as_rvalue;
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::bidirectional_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::input_iterator_tag>);
    CHECK_EQ(std::ranges::size(view), 5);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr++, 0);
    CHECK_EQ(*curr++, 1);
    CHECK_EQ(*curr++, 2);
    CHECK_EQ(*curr++, 3);
    CHECK_EQ(*curr++, 4);
    CHECK_EQ(curr, std::ranges::end(view));
    curr--;
    curr--;
    curr--;
    curr--;
    curr--;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("random_access_range")
{
    static const int input[] = { 0, 1, 2, 3, 4 };
    auto view = input | views::as_rvalue;
    using view_type = decltype(view);
    static_assert(
        std::same_as<typename std::iterator_traits<
                         std::ranges::iterator_t<view_type>>::iterator_category,
                     std::random_access_iterator_tag>);
    static_assert(std::same_as<
                  typename std::ranges::iterator_t<view_type>::iterator_concept,
                  std::input_iterator_tag>);
    CHECK_EQ(std::ranges::size(view), 5);
    auto curr = std::ranges::begin(view);
    CHECK_EQ(*curr, 0);
    curr += 1;
    CHECK_EQ(*curr, 1);
    curr += 1;
    CHECK_EQ(*curr, 2);
    curr += 1;
    CHECK_EQ(*curr, 3);
    curr += 1;
    CHECK_EQ(*curr, 4);
    curr += 1;
    CHECK_EQ(curr, std::ranges::end(view));
    curr -= 5;
    CHECK_EQ(curr, std::ranges::begin(view));
}

TEST_CASE("glvalue")
{
    static const std::vector<std::string> expected
        = { "the", "quick", "brown", "fox", "ate", "a", "pterodactyl" };
    auto input = expected;
    std::vector<std::string> output;
    std::ranges::copy(input | views::as_rvalue, std::back_inserter(output));
    CHECK_EQ(output, expected);
    for (auto&& element : input) {
        CHECK(element.empty());
    }
}

TEST_CASE("prvalue")
{
    std::vector<std::string> output;
    std::ranges::copy(std::views::iota(0, 5) | views::as_rvalue
                          | std::views::transform([](auto&& value) {
                                return std::to_string(value);
                            }),
                      std::back_inserter(output));
    CHECK_EQ(output, std::vector<std::string> { "0", "1", "2", "3", "4" });
}

TEST_SUITE_END();
