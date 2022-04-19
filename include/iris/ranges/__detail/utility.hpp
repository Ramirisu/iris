#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges::__detail {

// clang-format off
template <typename Range>
concept __simple_view = std::ranges::view<Range> 
    && std::ranges::range<Range> 
    && std::same_as<
        std::ranges::iterator_t<Range>, 
        std::ranges::iterator_t<const Range>> 
    && std::same_as<
        std::ranges::sentinel_t<Range>, 
        std::ranges::sentinel_t<const Range>>;
// clang-format on

template <typename T>
using __with_reference = T&;

template <typename T>
concept __can_reference = requires
{
    typename __with_reference<T>;
};

template <bool IsConst, typename T>
using __maybe_const = std::conditional_t<IsConst, const T, T>;

}
