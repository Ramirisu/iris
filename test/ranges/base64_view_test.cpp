#include <thirdparty/test.hpp>

#include <iris/ranges/base64_view.hpp>

#include <algorithm>
#include <string_view>

using namespace iris;

TEST_SUITE_BEGIN("ranges/base64_view");

struct test_case_t {
    std::string_view binary;
    std::string_view text;
};

static const auto test_cases = std::vector<test_case_t> {
    { "Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu" },
    { "Many hands make light work", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcms=" },
    { "Many hands make light work..",
      "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsuLg==" }
};

TEST_CASE("base64_encode")
{
    for (auto& test_case : test_cases) {
        CHECK(std::ranges::equal(views::base64_encode(test_case.binary),
                                 test_case.text));
    }
}

TEST_CASE("base64_decode")
{
    for (auto& test_case : test_cases) {
        CHECK(std::ranges::equal(views::base64_decode(test_case.text),
                                 test_case.binary));
    }
}

static const auto encode_twice
    = test_case_t { "Many hands make light work.",
                    "VFdGdWVTQm9ZVzVrY3lCdFlXdGxJR3hwWjJoMElIZHZjbXN1" };

TEST_CASE("pipe")
{
    auto unwrap = std::views::transform([](auto exp) { return exp.value(); });
    CHECK(std::ranges::equal(encode_twice.binary | views::base64_encode | unwrap
                                 | views::base64_encode,
                             encode_twice.text));
    CHECK(std::ranges::equal(encode_twice.text | views::base64_decode | unwrap
                                 | views::base64_decode,
                             encode_twice.binary));
}

TEST_SUITE_END();
