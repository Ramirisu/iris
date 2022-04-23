#include <thirdparty/test.hpp>

#include <iris/ranges/as_rvalue_view.hpp>

using namespace iris;

TEST_SUITE_BEGIN("as_rvalue_view");

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
