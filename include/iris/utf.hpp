#pragma once

#include <iris/config.hpp>

#include <iris/expected.hpp>

#include <concepts>

namespace iris {

enum class utf_error {
    none,
    eof,
    incomplete,
    illegal_character,
};

template <typename Unicode, typename UTF, std::size_t = sizeof(UTF)>
class utf;

template <typename T, std::size_t N>
class utf_result {
public:
    utf_result() = default;

    template <typename... Ts>
    utf_result(Ts&&... ts) requires(sizeof...(Ts) <= N)
        : data_ { static_cast<T>(std::forward<Ts>(ts))... }
        , size_(sizeof...(Ts))
    {
    }

    const T& operator[](std::size_t index) const noexcept
    {
        return data_[index];
    }

    std::size_t size() const noexcept
    {
        return size_;
    }

private:
    T data_[N] {};
    std::size_t size_ = 0;
};

template <typename Unicode, typename UTF>
class utf<Unicode, UTF, 1> {
public:
    template <typename Iterator, typename Sentinel>
    static constexpr expected<Unicode, utf_error>
    decode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(utf_error::eof);
        }

        std::uint8_t lead = *first++;
        auto size = utf8_size(lead);
        if (size == 0) {
            return unexpected(utf_error::illegal_character);
        }

        std::uint32_t codepoint = lead & ((1 << (8 - size)) - 1);

        switch (size) {
        case 4: {
            if (first == last) {
                return unexpected(utf_error::incomplete);
            }
            std::uint8_t byte = *first++;
            if (!is_utf8_traling_byte(byte)) {
                return unexpected(utf_error::illegal_character);
            }
            codepoint = (codepoint << 6) | (byte & 0x3f);
            [[fallthrough]];
        }
        case 3: {
            if (first == last) {
                return unexpected(utf_error::incomplete);
            }
            std::uint8_t byte = *first++;
            if (!is_utf8_traling_byte(byte)) {
                return unexpected(utf_error::illegal_character);
            }
            codepoint = (codepoint << 6) | (byte & 0x3f);
            [[fallthrough]];
        }
        case 2: {
            if (first == last) {
                return unexpected(utf_error::incomplete);
            }
            std::uint8_t byte = *first++;
            if (!is_utf8_traling_byte(byte)) {
                return unexpected(utf_error::illegal_character);
            }
            codepoint = (codepoint << 6) | (byte & 0x3f);
            [[fallthrough]];
        }
        default:
            break;
        }

        return codepoint;
    }

    template <typename Iterator, typename Sentinel>
    static constexpr expected<utf_result<UTF, 4>, utf_error>
    encode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(utf_error::eof);
        }

        if (*first <= 0x7f) {
            auto b0 = *first++;
            return utf_result<UTF, 4> { b0 };
        } else if (*first <= 0x07ff) {
            auto b0 = ((*first >> 6) & 0x1f) | 0xc0;
            auto b1 = (*first++ & 0x3f) | 0x80;
            return utf_result<UTF, 4> { b0, b1 };
        } else if (*first <= 0xffff) {
            auto b0 = ((*first >> 12) & 0xff) | 0xe0;
            auto b1 = ((*first >> 6) & 0x3f) | 0x80;
            auto b2 = (*first++ & 0x3f) | 0x80;
            return utf_result<UTF, 4> { b0, b1, b2 };
        } else if (*first <= 0x1fffff) {
            auto b0 = ((*first >> 18) & 0x7) | 0xf0;
            auto b1 = ((*first >> 12) & 0x3f) | 0x80;
            auto b2 = ((*first >> 6) & 0x3f) | 0x80;
            auto b3 = (*first++ & 0x3f) | 0x80;
            return utf_result<UTF, 4> { b0, b1, b2, b3 };
        }

        ++first;
        return unexpected(utf_error::illegal_character);
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
class utf<Unicode, UTF, 2> {
public:
    template <typename Iterator, typename Sentinel>
    static constexpr expected<Unicode, utf_error>
    decode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(utf_error::eof);
        }

        std::uint16_t lead = *first++;
        if (lead <= 0xd7ff || lead >= 0xe000 && lead <= 0xffff) {
            return lead;
        } else if (is_lead_surrogates(lead)) {
            if (first == last) {
                return unexpected(utf_error::incomplete);
            }
            std::uint16_t trail = *first++;
            if (is_trail_surrogates(trail)) {
                return ((lead & 0x3ff) << 10) | (trail & 0x3ff);
            }
        }

        return unexpected(utf_error::illegal_character);
    }

    template <typename Iterator, typename Sentinel>
    static constexpr expected<utf_result<UTF, 2>, utf_error>
    encode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(utf_error::eof);
        }

        std::uint32_t codepoint = *first++;
        if (codepoint <= 0xd7ff || codepoint >= 0xe000 && codepoint <= 0xffff) {
            return utf_result<UTF, 2> { codepoint };
        } else if (codepoint >= 0x10000 && codepoint <= 0x10ffff) {
            codepoint -= 0x10000;
            return utf_result<UTF, 2> { (codepoint >> 10) | 0xd800,
                                        (codepoint & 0x3ff) | 0xdc00 };
        }

        return unexpected(utf_error::illegal_character);
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
class utf<Unicode, UTF, 4> {
public:
    template <typename Iterator, typename Sentinel>
    static constexpr expected<Unicode, utf_error>
    decode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(utf_error::eof);
        }

        std::uint32_t lead = *first++;
        if (is_valid_codepoint(lead)) {
            return lead;
        }

        return unexpected(utf_error::illegal_character);
    }

    template <typename Iterator, typename Sentinel>
    static constexpr expected<utf_result<UTF, 2>, utf_error>
    encode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(utf_error::eof);
        }

        std::uint32_t lead = *first++;
        if (is_valid_codepoint(lead)) {
            return lead;
        }

        return unexpected(utf_error::illegal_character);
    }

private:
    static constexpr bool is_valid_codepoint(std::uint32_t value) noexcept
    {
        return value <= 0xd7ff || (value >= 0xe000 && value <= 0x10ffff);
    }
};

}
