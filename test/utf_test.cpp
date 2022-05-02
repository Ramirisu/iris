#include <thirdparty/test.hpp>

#include <iris/utf.hpp>

using namespace iris;

TEST_SUITE_BEGIN("utf");

static const auto utf8_cstr = std::string_view("IRIS伊莉絲");
static const auto utf8_str = std::u8string_view(u8"IRIS伊莉絲");
static const auto utf16_str = std::u16string_view(u"IRIS伊莉絲");
static const auto utf32_str = std::u32string_view(U"IRIS伊莉絲");
static const auto unicode
    = std::vector<std::uint32_t> { 0x49,   0x52,   0x49,  0x53,
                                   0x4f0a, 0x8389, 0x7d72 };

template <typename Unicode,
          typename UTF,
          typename InputRange,
          typename ExpectedRange>
void test_utf_decode(InputRange input, ExpectedRange exp)
{
    auto first = std::ranges::begin(input);
    auto last = std::ranges::end(input);
    auto iter = std::ranges::begin(exp);
    while (first != last) {
        auto result = utf<Unicode, UTF>::decode(first, last);
        CHECK(result);
        CHECK_EQ(result.value(), *iter++);
    }
}

template <typename Unicode,
          typename UTF,
          typename InputRange,
          typename ExpectedRange>
void test_utf_encode(InputRange input, ExpectedRange exp)
{
    auto first = std::ranges::begin(input);
    auto last = std::ranges::end(input);
    auto iter = std::ranges::begin(exp);
    while (first != last) {
        auto result = utf<Unicode, UTF>::encode(first, last);
        CHECK(result);
        const auto& value = result.value();
        for (std::size_t i = 0; i < value.size(); ++i) {
            CHECK_EQ(static_cast<std::make_unsigned_t<UTF>>(value[i]),
                     static_cast<std::make_unsigned_t<std::remove_cvref_t<
                         std::ranges::range_value_t<ExpectedRange>>>>(*iter++));
        }
    }
}

TEST_CASE("utf: utf-8")
{
    test_utf_decode<char32_t, char8_t>(utf8_cstr, unicode);
    test_utf_decode<char32_t, char>(utf8_cstr, unicode);
    test_utf_decode<char32_t, std::uint8_t>(utf8_cstr, unicode);
    test_utf_decode<std::uint32_t, char8_t>(utf8_cstr, unicode);
    test_utf_decode<std::uint32_t, char>(utf8_cstr, unicode);
    test_utf_decode<std::uint32_t, std::uint8_t>(utf8_cstr, unicode);
    test_utf_decode<char32_t, char8_t>(utf8_str, unicode);
    test_utf_decode<char32_t, char>(utf8_str, unicode);
    test_utf_decode<char32_t, std::uint8_t>(utf8_str, unicode);
    test_utf_decode<std::uint32_t, char8_t>(utf8_str, unicode);
    test_utf_decode<std::uint32_t, char>(utf8_str, unicode);
    test_utf_decode<std::uint32_t, std::uint8_t>(utf8_str, unicode);

    test_utf_encode<char32_t, char8_t>(unicode, utf8_cstr);
    test_utf_encode<char32_t, char>(unicode, utf8_cstr);
    test_utf_encode<char32_t, std::uint8_t>(unicode, utf8_cstr);
    test_utf_encode<std::uint32_t, char8_t>(unicode, utf8_cstr);
    test_utf_encode<std::uint32_t, char>(unicode, utf8_cstr);
    test_utf_encode<std::uint32_t, std::uint8_t>(unicode, utf8_cstr);
    test_utf_encode<char32_t, char8_t>(unicode, utf8_str);
    test_utf_encode<char32_t, char>(unicode, utf8_str);
    test_utf_encode<char32_t, std::uint8_t>(unicode, utf8_str);
    test_utf_encode<std::uint32_t, char8_t>(unicode, utf8_str);
    test_utf_encode<std::uint32_t, char>(unicode, utf8_str);
    test_utf_encode<std::uint32_t, std::uint8_t>(unicode, utf8_str);
}

TEST_CASE("utf: utf-16")
{
    test_utf_decode<char32_t, char16_t>(utf16_str, unicode);
    test_utf_decode<char32_t, std::uint16_t>(utf16_str, unicode);
    test_utf_decode<std::uint32_t, char16_t>(utf16_str, unicode);
    test_utf_decode<std::uint32_t, std::uint16_t>(utf16_str, unicode);

    test_utf_encode<char32_t, char16_t>(unicode, utf16_str);
    test_utf_encode<char32_t, std::uint16_t>(unicode, utf16_str);
    test_utf_encode<std::uint32_t, char16_t>(unicode, utf16_str);
    test_utf_encode<std::uint32_t, std::uint16_t>(unicode, utf16_str);
}

TEST_CASE("utf: utf-32")
{
    test_utf_decode<char32_t, char32_t>(utf32_str, unicode);
    test_utf_decode<char32_t, std::uint32_t>(utf32_str, unicode);
    test_utf_decode<std::uint32_t, char32_t>(utf32_str, unicode);
    test_utf_decode<std::uint32_t, std::uint32_t>(utf32_str, unicode);

    test_utf_encode<char32_t, char32_t>(unicode, utf32_str);
    test_utf_encode<char32_t, std::uint32_t>(unicode, utf32_str);
    test_utf_encode<std::uint32_t, char32_t>(unicode, utf32_str);
    test_utf_encode<std::uint32_t, std::uint32_t>(unicode, utf32_str);
}

TEST_SUITE_END();
