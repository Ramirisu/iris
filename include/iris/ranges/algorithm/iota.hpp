#pragma once

#include <iris/config.hpp>

#include <iris/ranges/algorithm/base.hpp>

#include <ranges>

namespace iris::ranges {

template <class O, class T>
using iota_result = out_value_result<O, T>;

struct __iota_fn {
    template <std::input_or_output_iterator O,
              std::sentinel_for<O> S,
              std::weakly_incrementable T>
        requires std::indirectly_writable<O, const T&>
    constexpr iota_result<O, T> operator()(O first, S last, T value) const
    {
        while (first != last) {
            *first = std::as_const(value);
            ++first;
            ++value;
        }

        return { std::move(first), std::move(value) };
    }

    template <std::weakly_incrementable T,
              std::ranges::output_range<const T&> R>
    constexpr iota_result<std::ranges::borrowed_iterator_t<R>, T>
    operator()(R&& r, T value) const
    {
        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), std::move(value));
    }
};

inline constexpr __iota_fn iota {};

}
