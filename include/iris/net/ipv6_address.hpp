#pragma once

#include <iris/config.hpp>

#include <cstdint>
#include <string>

namespace iris::net {

class ipv6_address {
    alignas(16) std::uint8_t bytes_[16];

public:
    ipv6_address()
        : bytes_()
    {
    }

    ipv6_address(std::uint16_t p0,
                 std::uint16_t p1,
                 std::uint16_t p2,
                 std::uint16_t p3,
                 std::uint16_t p4,
                 std::uint16_t p5,
                 std::uint16_t p6,
                 std::uint16_t p7)
        : bytes_ {
            static_cast<std::uint8_t>(p0 >> 8), static_cast<std::uint8_t>(p0),
            static_cast<std::uint8_t>(p1 >> 8), static_cast<std::uint8_t>(p1),
            static_cast<std::uint8_t>(p2 >> 8), static_cast<std::uint8_t>(p2),
            static_cast<std::uint8_t>(p3 >> 8), static_cast<std::uint8_t>(p3),
            static_cast<std::uint8_t>(p4 >> 8), static_cast<std::uint8_t>(p4),
            static_cast<std::uint8_t>(p5 >> 8), static_cast<std::uint8_t>(p5),
            static_cast<std::uint8_t>(p6 >> 8), static_cast<std::uint8_t>(p6),
            static_cast<std::uint8_t>(p7 >> 8), static_cast<std::uint8_t>(p7),
        }
    {
    }

    std::string to_string() const noexcept
    {
        std::size_t longest_index = -1;
        std::size_t longest_size = 0;
        for (std::size_t index = 0; index < 8;) {
            std::size_t size = 0;
            while (index + size < 8 && bytes_[2 * (index + size)] == 0
                   && bytes_[2 * (index + size) + 1] == 0) {
                ++size;
            }
            if (size > longest_size) {
                longest_index = index;
                longest_size = size;
            }
            index += size + 1;
        }

        std::string result;
        std::string separator;

        auto to_hex_char = [](std::uint8_t byte) {
            return byte >= 10 ? byte + 'a' - 10 : byte + '0';
        };

        auto append_separator = [&]() {
            result += separator;
            separator = ":";
        };

        auto append_part = [&](std::size_t index) {
            auto hi = bytes_[2 * index];
            auto lo = bytes_[2 * index + 1];
            if (hi > 0xf) {
                result += to_hex_char(hi >> 4);
                result += to_hex_char(hi & 0xf);
                result += to_hex_char(lo >> 4);
            } else if (hi > 0x0) {
                result += to_hex_char(hi & 0xf);
                result += to_hex_char(lo >> 4);
            } else if (lo > 0xf) {
                result += to_hex_char(lo >> 4);
            }
            result += to_hex_char(lo & 0xf);
        };

        for (std::size_t index = 0; index < 8;) {
            append_separator();

            if (longest_size >= 2 && index == longest_index) {
                if (index == 0) {
                    append_separator();
                }
                if (index + longest_size >= 8) {
                    append_separator();
                }
                index += longest_size;
            } else {
                append_part(index);
                ++index;
            }
        }

        return result;
    }
};

}
