#include <thirdparty/test.hpp>

#include <iris/ranges/view/base64_view.hpp>

#include <algorithm>
#include <string_view>

using namespace iris;

TEST_SUITE_BEGIN("ranges/[to|from]_base64_view");

struct test_case_t {
    std::string_view binary;
    std::string_view text;
};

static const auto test_cases = std::vector<test_case_t> {
    { "", "" },
    { "Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu" },
    { "Many hands make light work", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcms=" },
    { "Many hands make light work..",
      "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsuLg==" }
};

TEST_CASE("to_base64")
{
    for (auto& test_case : test_cases) {
        CHECK(std::ranges::equal(test_case.binary | views::to_base64,
                                 test_case.text));
    }
}

TEST_CASE("from_base64")
{
    for (auto& test_case : test_cases) {
        CHECK(std::ranges::equal(test_case.text | views::from_base64,
                                 test_case.binary));
    }
}

static const auto encode_twice_test_cases = std::vector<test_case_t> {
    { "", "" },
    { "Many hands make light work.",
      "VFdGdWVTQm9ZVzVrY3lCdFlXdGxJR3hwWjJoMElIZHZjbXN1" }
};

TEST_CASE("twice")
{
    for (auto& test_case : encode_twice_test_cases) {
        CHECK(std::ranges::equal(
            test_case.binary | views::to_base64
                | std::views::transform([](auto value) {
                      return value;
                  }) // to prevent from invoking to_base64_view's move ctor
                | views::to_base64,
            test_case.text));
        CHECK(std::ranges::equal(
            test_case.text | views::from_base64
                | std::views::transform([](auto exp) { return exp.value(); })
                | views::from_base64,
            test_case.binary));
    }
}

TEST_SUITE_END();
