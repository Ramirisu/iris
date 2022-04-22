#pragma once

#include <iris/config.hpp>

#include <iris/algorithm/base.hpp>

#include <ranges>

namespace iris::ranges {

struct __ends_with_fn {
    template <std::input_iterator I1,
              std::sentinel_for<I1> S1,
              std::input_iterator I2,
              std::sentinel_for<I2> S2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires(std::forward_iterator<I1> || std::sized_sentinel_for<S1, I1>)
    &&(std::forward_iterator<I2> || std::sized_sentinel_for<S2, I2>)&&std::
        indirectly_comparable<I1, I2, Pred, Proj1, Proj2> constexpr bool
        operator()(I1 first1,
                   S1 last1,
                   I2 first2,
                   S2 last2,
                   Pred pred = {},
                   Proj1 proj1 = {},
                   Proj2 proj2 = {}) const
    {
        const auto n1 = last1 - first1;
        const auto n2 = last2 - first2;

        if (n1 < n2) {
            return false;
        }

        return std::ranges::equal(std::move(first1) + (n1 - n2), last1,
                                  std::move(first2), last2, pred, proj1, proj2);
    }

    template <std::ranges::input_range R1,
              std::ranges::input_range R2,
              class Pred = std::ranges::equal_to,
              class Proj1 = std::identity,
              class Proj2 = std::identity>
        requires(std::ranges::forward_range<R1> || std::ranges::sized_range<R1>)
    &&(std::ranges::forward_range<R2> || std::ranges::sized_range<R2>)&&std::
        indirectly_comparable<std::ranges::iterator_t<R1>,
                              std::ranges::iterator_t<R2>,
                              Pred,
                              Proj1,
                              Proj2> constexpr bool
        operator()(R1&& r1,
                   R2&& r2,
                   Pred pred = {},
                   Proj1 proj1 = {},
                   Proj2 proj2 = {}) const
    {
        const auto n1 = std::ranges::distance(r1);
        const auto n2 = std::ranges::distance(r2);

        if (n1 < n2) {
            return false;
        }

        return std::ranges::equal(
            std::ranges::drop_view(std::ranges::ref_view(r1), n1 - n2), r2,
            pred, proj1, proj2);
    }
};

inline constexpr __ends_with_fn ends_with = {};

}
