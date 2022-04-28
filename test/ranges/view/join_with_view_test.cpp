#include <thirdparty/test.hpp>

#include <iris/ranges/view/join_with_view.hpp>

using namespace iris;

TEST_SUITE_BEGIN("join_with_view");

TEST_CASE("glvalue inner range")
{
    static const auto expected = std::string_view("the-quick-brown-fox");
    static const auto pattern = std::string_view("-");
    std::vector<std::string> range = { "the", "quick", "brown", "fox" };
    CHECK(std::ranges::equal(range | views::join_with(pattern), expected));
}

TEST_CASE("non-glvalue inner range")
{
    static const auto expected = std::string_view("the-quick-brown-fox");
    static const auto pattern = std::string_view("-");
    CHECK(std::ranges::equal(expected | std::views::split(pattern)
                                 | views::join_with(pattern),
                             expected));
}

TEST_SUITE_END();
