#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {

template <std::ranges::range Range,
          typename Allocator = std::allocator<std::byte>>
struct elements_of {
    Range range;
    Allocator allocator {};
};

template <typename Range, typename Allocator = std::allocator<std::byte>>
elements_of(Range&&, Allocator = {}) -> elements_of<Range&&, Allocator>;

}
