
#include <iris/ranges/to.hpp>
#include <iris/ranges/view/unwrap_view.hpp>
#include <iris/ranges/view/utf_view.hpp>

using namespace iris;

auto drop_invalid_characters()
{
    return std::views::filter([](auto&& exp) { return exp.has_value(); })
        | views::unwrap;
}

auto replace_invalid_characters_with_0xfffd()
{
    return std::views::transform(
        [](auto&& exp) { return exp.value_or(0xfffd); });
}

int main()
{
    static const auto utf8 = std::u8string_view(u8"str 好");
    static const auto utf16 = std::u16string_view(u"str 好");

    {
        // convert utf-8 to utf-16 and unwrap the value directly, throws if
        // there is any invalid character.
        auto result = utf8 | views::from_utf | views::unwrap
            | views::to_utf<char16_t> | views::unwrap
            | ranges::to<std::u16string>();
        IRIS_ASSERT(result == utf16);
    }
    {
        // convert utf-8 to utf-16 and drop all invalid characters.
        auto result = utf8 | views::from_utf | drop_invalid_characters()
            | views::to_utf<char16_t> | views::unwrap
            | ranges::to<std::u16string>();
        IRIS_ASSERT(result == utf16);
    }
    {
        // convert utf-8 to utf-16 and convert all invalid characters to U+FFFD.
        auto result = utf8 | views::from_utf
            | replace_invalid_characters_with_0xfffd()
            | views::to_utf<char16_t> | views::unwrap
            | ranges::to<std::u16string>();
        IRIS_ASSERT(result == utf16);
    }

    return 0;
}
