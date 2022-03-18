#pragma once

#include <iris/config.hpp>

#include <iris/net/ipv4_address.hpp>

#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

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

    static std::optional<ipv6_address>
    from_string(std::string_view addr) noexcept
    {
        auto parse_partial = [](std::string_view addr)
            -> std::optional<std::vector<std::uint16_t>> {
            std::vector<std::uint16_t> dbytes;
            auto curr_ptr = addr.data();
            auto last_ptr = addr.data() + addr.size();
            while (curr_ptr < last_ptr) {
                std::uint16_t dbyte = 0;
                if (auto [ptr, ec]
                    = std::from_chars(curr_ptr, last_ptr, dbyte, 16);
                    ec == std::errc()) {
                    if (ptr < last_ptr && *ptr != ':') {
                        return std::nullopt;
                    }
                    dbytes.push_back(dbyte);
                    curr_ptr = ptr + 1;
                } else {
                    return std::nullopt;
                }
            }

            if (curr_ptr != last_ptr + 1) {
                return std::nullopt;
            }

            return dbytes;
        };

        // TODO: hybrid dual-stack ipv6/ipv4
        // "hhhh:hhhh:hhhh:hhhh:hhhh:hhhh:111.111.111.111

        if (auto pos = addr.find("::"); pos != std::string_view::npos) {
            auto front = parse_partial(addr.substr(0, pos));
            auto back = parse_partial(addr.substr(pos + 2));
            if (front && back && front->size() + back->size() <= 7) {
                front->resize(8 - back->size());
                front->insert(std::end(*front), std::begin(*back),
                              std::end(*back));
                const auto& bytes = front.value();
                return ipv6_address(bytes[0], bytes[1], bytes[2], bytes[3],
                                    bytes[4], bytes[5], bytes[6], bytes[7]);
            }
        } else {
            if (auto result = parse_partial(addr);
                result && result->size() == 8) {
                const auto& bytes = result.value();
                return ipv6_address(bytes[0], bytes[1], bytes[2], bytes[3],
                                    bytes[4], bytes[5], bytes[6], bytes[7]);
            }
        }

        return std::nullopt;
    }
};

}
