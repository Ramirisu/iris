#pragma once

#include <iris/config.hpp>

#include <iris/__detail/static_storage.hpp>
#include <iris/expected.hpp>

#include <concepts>

namespace iris::__detail {

enum class __utf_error {
    eof = 1,
    incomplete,
    illegal_character,
};

template <typename Unicode, typename UTF, std::size_t = sizeof(UTF)>
class __utf;

template <typename T>
using __utf8_code_units = __static_storage<T, 4>;

template <typename T>
using __utf16_code_units = __static_storage<T, 2>;

template <typename T>
using __utf32_code_units = __static_storage<T, 1>;

template <typename Unicode, typename UTF>
class __utf<Unicode, UTF, 1> {
public:
    static_assert(std::is_unsigned_v<
                      std::remove_cvref_t<Unicode>> && (sizeof(Unicode) == 4),
                  "Unicode should be unsigned interger with 4 bytes size.");

    using utf_result_type = expected<__utf8_code_units<UTF>, __utf_error>;
    using unicode_result_type = expected<Unicode, __utf_error>;

    template <std::input_iterator I, std::sentinel_for<I> S>
    static constexpr utf_result_type encode_next(I& first,
                                                 const S& last) noexcept
    {
        if (first == last) {
            return unexpected(__utf_error::eof);
        }

        std::uint32_t codepoint = *first++;
        if (codepoint <= 0x7f) {
            auto b0 = codepoint;
            return __utf8_code_units<UTF> { b0 };
        } else if (codepoint <= 0x07ff) {
            auto b0 = ((codepoint >> 6) & 0x1f) | 0xc0;
            auto b1 = (codepoint & 0x3f) | 0x80;
            return __utf8_code_units<UTF> { b0, b1 };
        } else if (codepoint <= 0xffff) {
            auto b0 = ((codepoint >> 12) & 0xff) | 0xe0;
            auto b1 = ((codepoint >> 6) & 0x3f) | 0x80;
            auto b2 = (codepoint & 0x3f) | 0x80;
            return __utf8_code_units<UTF> { b0, b1, b2 };
        } else if (codepoint <= 0x1fffff) {
            auto b0 = ((codepoint >> 18) & 0x7) | 0xf0;
            auto b1 = ((codepoint >> 12) & 0x3f) | 0x80;
            auto b2 = ((codepoint >> 6) & 0x3f) | 0x80;
            auto b3 = (codepoint & 0x3f) | 0x80;
            return __utf8_code_units<UTF> { b0, b1, b2, b3 };
        }

        ++first;
        return unexpected(__utf_error::illegal_character);
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
    static constexpr unicode_result_type decode_next(I& first,
                                                     const S& last) noexcept
    {
        if (first == last) {
            return unexpected(__utf_error::eof);
        }

        std::uint8_t lead = *first++;
        auto size = utf8_size(lead);
        if (size == 0) {
            return unexpected(__utf_error::illegal_character);
        }

        std::uint32_t codepoint = lead & ((1 << (8 - size)) - 1);

        switch (size) {
        case 4: {
            if (first == last) {
                return unexpected(__utf_error::incomplete);
            }
            std::uint8_t byte = *first++;
            if (!is_utf8_traling_byte(byte)) {
                return unexpected(__utf_error::illegal_character);
            }
            codepoint = (codepoint << 6) | (byte & 0x3f);
            [[fallthrough]];
        }
        case 3: {
            if (first == last) {
                return unexpected(__utf_error::incomplete);
            }
            std::uint8_t byte = *first++;
            if (!is_utf8_traling_byte(byte)) {
                return unexpected(__utf_error::illegal_character);
            }
            codepoint = (codepoint << 6) | (byte & 0x3f);
            [[fallthrough]];
        }
        case 2: {
            if (first == last) {
                return unexpected(__utf_error::incomplete);
            }
            std::uint8_t byte = *first++;
            if (!is_utf8_traling_byte(byte)) {
                return unexpected(__utf_error::illegal_character);
            }
            codepoint = (codepoint << 6) | (byte & 0x3f);
            [[fallthrough]];
        }
        default:
            break;
        }

        return codepoint;
    }

private:
    static constexpr std::size_t utf8_size(std::uint8_t value) noexcept
    {
        if ((value & 0x80) == 0x0) {
            return 1;
        }
        if ((value & 0xe0) == 0xc0) {
            return 2;
        }
        if ((value & 0xf0) == 0xe0) {
            return 3;
        }
        if ((value & 0xf8) == 0xf0) {
            return 4;
        }

        return 0;
    }

    static constexpr bool is_utf8_traling_byte(std::uint8_t value) noexcept
    {
        return (value & 0xc0) == 0x80;
    }
};

template <typename Unicode, typename UTF>
class __utf<Unicode, UTF, 2> {
public:
    static_assert(std::is_unsigned_v<
                      std::remove_cvref_t<Unicode>> && (sizeof(Unicode) == 4),
                  "Unicode should be unsigned interger with 4 bytes size.");

    using utf_result_type = expected<__utf16_code_units<UTF>, __utf_error>;
    using unicode_result_type = expected<Unicode, __utf_error>;

    template <std::input_iterator I, std::sentinel_for<I> S>
    static constexpr utf_result_type encode_next(I& first,
                                                 const S& last) noexcept
    {
        if (first == last) {
            return unexpected(__utf_error::eof);
        }

        std::uint32_t codepoint = *first++;
        if (codepoint <= 0xd7ff
            || (codepoint >= 0xe000 && codepoint <= 0xffff)) {
            return __utf16_code_units<UTF> { codepoint };
        } else if (codepoint >= 0x10000 && codepoint <= 0x10ffff) {
            codepoint -= 0x10000;
            return __utf16_code_units<UTF> { (codepoint >> 10) | 0xd800,
                                             (codepoint & 0x3ff) | 0xdc00 };
        }

        return unexpected(__utf_error::illegal_character);
    }

    template <std::input_iterator I, std::sentinel_for<I> S>
    static constexpr unicode_result_type decode_next(I& first,
                                                     const S& last) noexcept
    {
        if (first == last) {
            return unexpected(__utf_error::eof);
        }

        std::uint16_t lead = *first++;
        if (lead <= 0xd7ff || lead >= 0xe000) {
            return lead;
        } else if (is_lead_surrogates(lead)) {
            if (first == last) {
                return unexpected(__utf_error::incomplete);
            }
            std::uint16_t trail = *first++;
            if (is_trail_surrogates(trail)) {
                return ((lead & 0x3ff) << 10) | (trail & 0x3ff);
            }
        }

        return unexpected(__utf_error::illegal_character);
    }

private:
    static constexpr bool is_lead_surrogates(std::uint16_t value) noexcept
    {
        return value >= 0xd800 && value <= 0xdbff;
    }

    static constexpr bool is_trail_surrogates(std::uint16_t value) noexcept
    {
        return value >= 0xdc00 && value <= 0xdfff;
    }
};

template <typename Unicode, typename UTF>
class __utf<Unicode, UTF, 4> {
public:
    static_assert(std::is_unsigned_v<
                      std::remove_cvref_t<Unicode>> && (sizeof(Unicode) == 4),
                  "Unicode should be unsigned interger with 4 bytes size.");

    using utf_result_type = expected<__utf32_code_units<UTF>, __utf_error>;
    using unicode_result_type = expected<Unicode, __utf_error>;

    template <std::input_or_output_iterator I, std::sentinel_for<I> S>
    static constexpr utf_result_type encode_next(I& first,
                                                 const S& last) noexcept
    {
        if (first == last) {
            return unexpected(__utf_error::eof);
        }

        std::uint32_t lead = *first++;
        if (is_valid_codepoint(lead)) {
            return __utf32_code_units<UTF> { lead };
        }

        return unexpected(__utf_error::illegal_character);
    }

    template <std::input_or_output_iterator I, std::sentinel_for<I> S>
    static constexpr unicode_result_type decode_next(I& first,
                                                     const S& last) noexcept
    {
        if (first == last) {
            return unexpected(__utf_error::eof);
        }

        std::uint32_t lead = *first++;
        if (is_valid_codepoint(lead)) {
            return lead;
        }

        return unexpected(__utf_error::illegal_character);
    }

private:
    static constexpr bool is_valid_codepoint(std::uint32_t value) noexcept
    {
        return value <= 0xd7ff || (value >= 0xe000 && value <= 0x10ffff);
    }
};

}
