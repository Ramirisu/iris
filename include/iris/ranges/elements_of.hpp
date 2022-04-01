#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {

template <std::ranges::range Range>
struct elements_of {
    Range range;
};

template <typename Range>
elements_of(Range&&) -> elements_of<Range&&>;

}
