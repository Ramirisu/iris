#include <thirdparty/test.hpp>

#include <iris/__detail/base64.hpp>

using namespace iris::__detail;

TEST_SUITE_BEGIN("base64");

struct test_cast_t {
    std::string_view binary;
    std::string_view text;
};

static const auto test_cases = std::vector<test_cast_t> {
    { "Many hands make light work.", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu" },
    { "Many hands make light work", "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcms=" },
    { "Many hands make light work..",
      "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsuLg==" }
};

template <typename Binary,
          typename Text,
          typename InputRange,
          typename ExpectedRange>
void test_base64_encode(InputRange input, ExpectedRange exp)
{
    auto first = std::ranges::begin(input);
    auto last = std::ranges::end(input);
    auto iter = std::ranges::begin(exp);
    while (first != last) {
        auto result = __base64<Binary, Text>::encode_next(first, last);
        CHECK(result);
        const auto& value = result.value();
        for (std::size_t i = 0; i < value.size(); ++i) {
            CHECK_EQ(value[i], *iter++);
        }
    }
}

template <typename Binary,
          typename Text,
          typename InputRange,
          typename ExpectedRange>
void test_base64_decode(InputRange input, ExpectedRange exp)
{
    auto first = std::ranges::begin(input);
    auto last = std::ranges::end(input);
    auto iter = std::ranges::begin(exp);
    while (first != last) {
        auto result = __base64<Binary, Text>::decode_next(first, last);
        CHECK(result);
        const auto& value = result.value();
        for (std::size_t i = 0; i < value.size(); ++i) {
            CHECK_EQ(value[i], *iter++);
        }
    }
}

TEST_CASE("base64 encode|decode")
{
    for (auto& test_case : test_cases) {
        test_base64_encode<char8_t, char8_t>(test_case.binary, test_case.text);
        test_base64_decode<char8_t, char8_t>(test_case.text, test_case.binary);
    }
}

TEST_SUITE_END();
