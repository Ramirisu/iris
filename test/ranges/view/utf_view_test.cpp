#include <thirdparty/test.hpp>

#include <iris/ranges/view/unwrap_view.hpp>
#include <iris/ranges/view/utf_view.hpp>

#include <algorithm>
#include <string>
#include <vector>

using namespace iris;

TEST_SUITE_BEGIN("ranges/utf_view");

static const auto utf8_cstr = std::string_view("IRIS伊莉絲");
static const auto utf8_str = std::u8string_view(u8"IRIS伊莉絲");
static const auto utf16_str = std::u16string_view(u"IRIS伊莉絲");
static const auto utf32_str = std::u32string_view(U"IRIS伊莉絲");
static const auto unicode
    = std::vector<std::uint32_t> { 0x49,   0x52,   0x49,  0x53,
                                   0x4f0a, 0x8389, 0x7d72 };

TEST_CASE("from_utf")
{
    CHECK(std::ranges::equal(utf8_cstr | views::from_utf | views::unwrap,
                             unicode));
    CHECK(std::ranges::equal(utf8_str | views::from_utf | views::unwrap,
                             unicode));
    CHECK(std::ranges::equal(utf16_str | views::from_utf | views::unwrap,
                             unicode));
    CHECK(std::ranges::equal(utf32_str | views::from_utf | views::unwrap,
                             unicode));
}

TEST_CASE("to_utf")
{
    CHECK(std::ranges::equal(unicode | views::to_utf<char> | views::unwrap,
                             utf8_cstr));
    CHECK(std::ranges::equal(unicode | views::to_utf<char8_t> | views::unwrap,
                             utf8_str));
    CHECK(std::ranges::equal(unicode | views::to_utf<char16_t> | views::unwrap,
                             utf16_str));
    CHECK(std::ranges::equal(unicode | views::to_utf<char32_t> | views::unwrap,
                             utf32_str));
}

TEST_CASE("utf8/16/32 conversion between one another")
{
    CHECK(std::ranges::equal(utf8_str | views::from_utf | views::unwrap
                                 | views::to_utf<char16_t> | views::unwrap,
                             utf16_str));
    CHECK(std::ranges::equal(utf8_str | views::from_utf | views::unwrap
                                 | views::to_utf<char32_t> | views::unwrap,
                             utf32_str));
    CHECK(std::ranges::equal(utf16_str | views::from_utf | views::unwrap
                                 | views::to_utf<char8_t> | views::unwrap,
                             utf8_str));
    CHECK(std::ranges::equal(utf16_str | views::from_utf | views::unwrap
                                 | views::to_utf<char32_t> | views::unwrap,
                             utf32_str));
    CHECK(std::ranges::equal(utf32_str | views::from_utf | views::unwrap
                                 | views::to_utf<char8_t> | views::unwrap,
                             utf8_str));
    CHECK(std::ranges::equal(utf32_str | views::from_utf | views::unwrap
                                 | views::to_utf<char16_t> | views::unwrap,
                             utf16_str));
}

TEST_SUITE_END();
