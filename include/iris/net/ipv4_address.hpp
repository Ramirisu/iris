#pragma once

#include <iris/config.hpp>

#include <cstdint>
#include <numeric>
#include <string>

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
};

}
