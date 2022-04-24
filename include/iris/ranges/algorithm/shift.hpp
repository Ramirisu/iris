#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {

struct __sfhit_left_fn {
    template <std::permutable I, std::sentinel_for<I> S>
    constexpr std::ranges::subrange<I>
    operator()(I first, S last, std::iter_difference_t<I> n) const
    {
        IRIS_ASSERT(n >= 0);

        if (n == 0) {
            return { first, last };
        }

        if (n >= last - first) {
            return { first, first };
        }

        auto target = first;
        auto source = std::ranges::next(target, n, last);
        while (source != last) {
            *target = std::move(*source);
            ++target;
            ++source;
        }

        return { first, target };
    }

    template <std::ranges::forward_range R>
        requires std::permutable<std::ranges::iterator_t<R>>
    constexpr std::ranges::borrowed_subrange_t<R>
    operator()(R&& r, std::ranges::range_difference_t<R> n) const
    {
        IRIS_ASSERT(n >= 0);

        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), n);
    }
};

inline constexpr __sfhit_left_fn shift_left {};

struct __shift_right_fn {
    template <std::permutable I, std::sentinel_for<I> S>
        requires std::bidirectional_iterator<I>
    constexpr std::ranges::subrange<I>
    operator()(I first, S last, std::iter_difference_t<I> n) const
    {
        IRIS_ASSERT(n >= 0);

        if (n == 0) {
            return { first, last };
        }

        if (n >= last - first) {
            return { last, last };
        }

        auto source = std::ranges::next(first, last - first - n - 1, last);
        auto target = std::ranges::next(source, n, last);
        while (source != first) {
            *target-- = std::move(*source--);
        }
        *target = std::move(*source);

        return { target, last };
    }

    template <std::ranges::forward_range R>
        requires std::permutable<std::ranges::iterator_t<R>>
    constexpr std::ranges::borrowed_subrange_t<R>
    operator()(R&& r, std::ranges::range_difference_t<R> n) const
    {
        IRIS_ASSERT(n >= 0);

        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), n);
    }
};

inline constexpr __shift_right_fn shift_right {};

}
