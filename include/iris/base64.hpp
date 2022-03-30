#pragma once

#include <iris/config.hpp>

#include <iris/detail/static_storage.hpp>
#include <iris/expected.hpp>

namespace iris {

enum class base64_error {
    eof = 1,
    incomplete,
    illegal_character,
};

template <typename T, std::size_t N>
using base64_result = detail::static_storage<T, N>;

template <typename Binary, typename Text>
class base64 {
public:
    using binary_result_type = expected<base64_result<Binary, 3>, base64_error>;
    using text_result_type = expected<base64_result<Text, 4>, base64_error>;

    template <typename Iterator, typename Sentinel>
    static constexpr expected<base64_result<Binary, 3>, base64_error>
    decode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(base64_error::eof);
        }

        std::uint8_t b0 = decode_table_[*first++];
        if (b0 == eq || b0 == err) {
            return unexpected(base64_error::illegal_character);
        }
        if (first == last) {
            return unexpected(base64_error::incomplete);
        }
        std::uint8_t b1 = decode_table_[*first++];
        if (b1 == eq || b1 == err) {
            return unexpected(base64_error::illegal_character);
        }
        if (first == last) {
            return unexpected(base64_error::incomplete);
        }
        std::uint8_t b2 = decode_table_[*first++];
        if (b2 == err) {
            return unexpected(base64_error::illegal_character);
        }
        if (first == last) {
            return unexpected(base64_error::incomplete);
        }
        std::uint8_t b3 = decode_table_[*first++];
        if (b3 == err) {
            return unexpected(base64_error::illegal_character);
        }

        if (b2 != eq && b3 != eq) {
            return base64_result<Binary, 3> { (b0 << 2) | (b1 >> 4),
                                              (b1 & 0xf) << 4 | (b2 >> 2),
                                              (b2 & 0x3) << 6 | b3 };
        } else if (b2 != eq && b3 == eq) {
            return base64_result<Binary, 3> { (b0 << 2) | (b1 >> 4),
                                              (b1 & 0xf) << 4 | (b2 >> 2) };
        } else if (b2 == eq && b3 == eq) {
            return base64_result<Binary, 3> { (b0 << 2) | (b1 >> 4) };
        }
        return unexpected(base64_error::illegal_character);
    }

    template <typename Iterator, typename Sentinel>
    static constexpr expected<base64_result<Text, 4>, base64_error>
    encode(Iterator& first, const Sentinel& last) noexcept
    {
        if (first == last) {
            return unexpected(base64_error::eof);
        }

        std::size_t padding = 0;
        std::uint32_t b = *first++ << 16;
        if (first == last) {
            return base64_result<Text, 4> {
                encode_table_[(b >> 18)],
                encode_table_[(b >> 12) & 0x3f],
                61,
                61,
            };
        }
        b |= *first++ << 8;
        if (first == last) {
            return base64_result<Text, 4> {
                encode_table_[(b >> 18)],
                encode_table_[(b >> 12) & 0x3f],
                encode_table_[(b >> 6) & 0x3f],
                61,
            };
        }
        b |= *first++;
        return base64_result<Text, 4> {
            encode_table_[(b >> 18)],
            encode_table_[(b >> 12) & 0x3f],
            encode_table_[(b >> 6) & 0x3f],
            encode_table_[(b >> 0) & 0x3f],
        };
    }

private:
    static inline constexpr std::uint8_t err = 255;
    static inline constexpr std::uint8_t eq = 254;
    static inline constexpr std::uint8_t decode_table_[256]
        = { err, err, err, err, err, err, err, err, err, err, // [0-9]
            err, err, err, err, err, err, err, err, err, err, // [10-19]
            err, err, err, err, err, err, err, err, err, err, // [20-29]
            err, err, err, err, err, err, err, err, err, err, // [30-39]
            err, err, err, 62,  err, err, err, 63,  52,  53, // [40-49]
            54,  55,  56,  57,  58,  59,  60,  61,  err, err, // [50-59]
            err, eq,  err, err, err, 0,   1,   2,   3,   4, // [60-69]
            5,   6,   7,   8,   9,   10,  11,  12,  13,  14, // [70-79]
            15,  16,  17,  18,  19,  20,  21,  22,  23,  24, // [80-89]
            25,  err, err, err, err, err, err, 26,  27,  28, // [90-99]
            29,  30,  31,  32,  33,  34,  35,  36,  37,  38, // [100-109]
            39,  40,  41,  42,  43,  44,  45,  46,  47,  48, // [110-119]
            49,  50,  51,  err, err, err, err, err, err, err, // [120-129]
            err, err, err, err, err, err, err, err, err, err, // [130-139]
            err, err, err, err, err, err, err, err, err, err, // [140-149]
            err, err, err, err, err, err, err, err, err, err, // [150-159]
            err, err, err, err, err, err, err, err, err, err, // [160-169]
            err, err, err, err, err, err, err, err, err, err, // [170-179]
            err, err, err, err, err, err, err, err, err, err, // [180-189]
            err, err, err, err, err, err, err, err, err, err, // [190-199]
            err, err, err, err, err, err, err, err, err, err, // [200-209]
            err, err, err, err, err, err, err, err, err, err, // [210-219]
            err, err, err, err, err, err, err, err, err, err, // [220-229]
            err, err, err, err, err, err, err, err, err, err, // [230-239]
            err, err, err, err, err, err, err, err, err, err, // [240-249]
            err, err, err, err, err, err }; // [250-255]
    static inline constexpr std::uint8_t encode_table_[64]
        = { 65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,
            78,  79,  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90,
            97,  98,  99,  100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
            110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
            48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  43,  47 };
};

}
