#pragma once

#include <iris/config.hpp>

#include <cstdint>
#include <numeric>
#include <string>

namespace iris::net {

class mac_address {
    alignas(8) std::uint8_t bytes_[6];

public:
    mac_address()
        : bytes_()
    {
    }

    mac_address(std::uint8_t b0,
                std::uint8_t b1,
                std::uint8_t b2,
                std::uint8_t b3,
                std::uint8_t b4,
                std::uint8_t b5)
        : bytes_ { b0, b1, b2, b3, b4, b5 }
    {
    }

    std::string to_string() const noexcept
    {
        auto to_hex_char = [](std::uint8_t byte) -> char {
            return byte >= 10 ? byte + 'a' - 10 : byte + '0';
        };
        std::string separator;
        return std::accumulate(std::begin(bytes_), std::end(bytes_),
                               std::string(), [&](auto accumulator, auto& b) {
                                   accumulator += separator;
                                   separator = ":";
                                   accumulator
                                       += to_hex_char(std::uint8_t(b >> 4));
                                   accumulator
                                       += to_hex_char(std::uint8_t(b & 0xf));
                                   return accumulator;
                               });
    }
};

}
