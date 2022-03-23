#pragma once

#include <iris/config.hpp>

#include <iris/expected.hpp>

#include <charconv>
#include <cstdint>
#include <numeric>
#include <string>
#include <system_error>

namespace iris::net {

class ipv4_address {
    alignas(4) std::uint8_t bytes_[4];

public:
    ipv4_address()
        : bytes_()
    {
    }

    explicit ipv4_address(std::uint32_t addr)
        : bytes_ {
            static_cast<std::uint8_t>(addr >> 24),
            static_cast<std::uint8_t>(addr >> 16),
            static_cast<std::uint8_t>(addr >> 8),
            static_cast<std::uint8_t>(addr >> 0),
        }
    {
    }

    explicit ipv4_address(std::uint8_t b0,
                          std::uint8_t b1,
                          std::uint8_t b2,
                          std::uint8_t b3)
        : bytes_ { b0, b1, b2, b3 }
    {
    }

    std::string to_string() const noexcept
    {
        std::string separator;
        return std::accumulate(std::begin(bytes_), std::end(bytes_),
                               std::string(), [&](auto accumulator, auto& b) {
                                   accumulator += separator;
                                   separator = ".";
                                   accumulator += std::to_string(b);
                                   return accumulator;
                               });
    }

    static expected<ipv4_address, std::error_code>
    from_string(std::string_view addr) noexcept
    {
        std::uint8_t bytes[4] = {};
        auto curr_ptr = addr.data();
        auto last_ptr = addr.data() + addr.size();
        for (int i = 0; i < 4 && curr_ptr < last_ptr; ++i) {
            if (auto [ptr, ec]
                = std::from_chars(curr_ptr, last_ptr, bytes[i], 10);
                ec == std::errc()) {
                curr_ptr = ptr + 1;
            } else {
                return iris::unexpected(
                    std::make_error_code(std::errc::invalid_argument));
            }
        }

        if (curr_ptr != last_ptr + 1) {
            return iris::unexpected(
                std::make_error_code(std::errc::invalid_argument));
        }

        return ipv4_address(bytes[0], bytes[1], bytes[2], bytes[3]);
    }
};

}
