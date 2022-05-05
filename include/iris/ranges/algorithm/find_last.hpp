#pragma once

#include <iris/config.hpp>

#include <functional>
#include <optional>
#include <ranges>

namespace iris::ranges {
namespace __find_last_detail {
    template <typename I, typename S, typename Pred, typename Proj>
    constexpr std::ranges::subrange<I>
    find_last(I first, S last, Pred pred, Proj proj, std::forward_iterator_tag)
    {
        std::optional<I> found;
        for (; first != last; ++first) {
            if (std::invoke(pred, std::invoke(proj, *first))) {
                found = first;
            }
        }
        if (found) {
            return { *found, last };
        } else {
            return { last, last };
        }
    }

    template <typename I, typename S, typename Pred, typename Proj>
    constexpr std::ranges::subrange<I> find_last(
        I first, S last, Pred pred, Proj proj, std::bidirectional_iterator_tag)
    {
        for (auto curr = last; curr != first;) {
            if (std::invoke(pred, std::invoke(proj, *--curr))) {
                return { curr, last };
            }
        }

        return { last, last };
    }
}

struct __find_last_fn {
    template <std::forward_iterator I,
              std::sentinel_for<I> S,
              class T,
              class Proj = std::identity>
        requires std::indirect_binary_predicate < std::ranges::equal_to,
            std::projected<I, Proj>,
    const T* > constexpr std::ranges::subrange<I>
        operator()(I first, S last, const T& value, Proj proj = {}) const
    {
        return __find_last_detail::find_last(
            std::move(first), std::move(last),
            [&value](const auto& input) { return value == input; },
            std::move(proj),
            typename std::iterator_traits<I>::iterator_category {});
    }

    template <std::ranges::forward_range R, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate < std::ranges::equal_to,
            std::projected<std::ranges::iterator_t<R>, Proj>,
    const T* > constexpr std::ranges::borrowed_subrange_t<R>
        operator()(R&& r, const T& value, Proj proj = {}) const
    {
        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), value,
                       std::move(proj));
    }
};

inline constexpr __find_last_fn find_last {};

struct __find_last_if_fn {
    template <std::forward_iterator I,
              std::sentinel_for<I> S,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::projected<I, Proj>> Pred>
    constexpr std::ranges::subrange<I>
    operator()(I first, S last, Pred pred, Proj proj = {}) const
    {
        return __find_last_detail::find_last(
            std::move(first), std::move(last), std::move(pred), std::move(proj),
            typename std::iterator_traits<I>::iterator_category {});
    }

    template <std::ranges::forward_range R,
              class Proj = std::identity,
              std::indirect_unary_predicate<
                  std::projected<std::ranges::iterator_t<R>, Proj>> Pred>
    constexpr std::ranges::borrowed_subrange_t<R>
    operator()(R&& r, Pred pred, Proj proj = {}) const
    {
        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), std::move(pred),
                       std::move(proj));
    }
};

inline constexpr __find_last_if_fn find_last_if {};

struct __find_last_if_not_fn {
    template <std::forward_iterator I,
              std::sentinel_for<I> S,
              class Proj = std::identity,
              std::indirect_unary_predicate<std::projected<I, Proj>> Pred>
    constexpr std::ranges::subrange<I>
    operator()(I first, S last, Pred pred, Proj proj = {}) const
    {
        return __find_last_detail::find_last(
            std::move(first), std::move(last),
            [pred = std::move(pred)](auto& value) { return !pred(value); },
            std::move(proj),
            typename std::iterator_traits<I>::iterator_category {});
    }

    template <std::ranges::forward_range R,
              class Proj = std::identity,
              std::indirect_unary_predicate<
                  std::projected<std::ranges::iterator_t<R>, Proj>> Pred>
    constexpr std::ranges::borrowed_subrange_t<R>
    operator()(R&& r, Pred pred, Proj proj = {}) const
    {
        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), std::move(pred),
                       std::move(proj));
    }
};

inline constexpr __find_last_if_not_fn find_last_if_not {};

}
