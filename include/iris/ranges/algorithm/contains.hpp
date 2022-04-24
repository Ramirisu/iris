#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {

struct __contains_fn {
    template <std::input_iterator I,
              std::sentinel_for<I> S,
              class T,
              class Proj = std::identity>
        requires std::indirect_binary_predicate < std::ranges::equal_to,
            std::projected<I, Proj>,
    const T* > constexpr bool
        operator()(I first, S last, const T& value, Proj proj = {}) const
    {
        return std::ranges::find(std::move(first), last, value, proj) != last;
    }

    template <std::ranges::input_range R, class T, class Proj = std::identity>
        requires std::indirect_binary_predicate < std::ranges::equal_to,
            std::projected<std::ranges::iterator_t<R>, Proj>,
    const T* > constexpr bool
        operator()(R&& r, const T& value, Proj proj = {}) const
    {
        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), value,
                       std::move(proj));
    }
};

inline constexpr __contains_fn contains = {};

struct __contains_subrange_fn {
    template <std::forward_iterator I1,
              std::sentinel_for<I1> S1,
              std::forward_iterator I2,
              std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires std::indirectly_comparable<I1, I2, Pred, Proj1, Proj2>
    constexpr bool operator()(I1 first1,
                              S1 last1,
                              I2 first2,
                              S2 last2,
                              Pred pred = {},
                              Proj1 proj1 = {},
                              Proj2 proj2 = {}) const
    {
        return first2 == last2
            || !std::ranges::search(std::move(first1), last1, std::move(first2),
                                    last2, std::move(pred), std::move(proj1),
                                    std::move(proj2))
                    .empty();
    }

    template <std::ranges::forward_range R1,
              std::ranges::forward_range R2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires std::indirectly_comparable<std::ranges::iterator_t<R1>,
                                            std::ranges::iterator_t<R2>,
                                            Pred,
                                            Proj1,
                                            Proj2>
    constexpr bool operator()(R1&& r1,
                              R2&& r2,
                              Pred pred = {},
                              Proj1 proj1 = {},
                              Proj2 proj2 = {}) const
    {
        return (*this)(std::ranges::begin(std::forward<R1>(r1)),
                       std::ranges::end(std::forward<R1>(r1)),
                       std::ranges::begin(std::forward<R2>(r2)),
                       std::ranges::end(std::forward<R2>(r2)), std::move(pred),
                       std::move(proj1), std::move(proj2));
    }
};

inline constexpr __contains_subrange_fn contains_subrange = {};

}
