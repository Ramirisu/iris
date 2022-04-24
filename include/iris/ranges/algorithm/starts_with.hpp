#pragma once

#include <iris/config.hpp>

#include <iris/ranges/algorithm/base.hpp>

#include <ranges>

namespace iris::ranges {

struct __starts_with_fn {
    template <std::input_iterator I1,
              std::sentinel_for<I1> S1,
              std::input_iterator I2,
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
        return std::ranges::mismatch(std::move(first1), last1,
                                     std::move(first2), last2, std::move(pred),
                                     std::move(proj1), std::move(proj2))
                   .in2
            == last2;
    }

    template <std::ranges::input_range R1,
              std::ranges::input_range R2,
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
        const auto last2 = std::ranges::end(r2);
        return std::ranges::mismatch(std::forward<R1>(r1), std::forward<R2>(r2),
                                     std::move(pred), std::move(proj1),
                                     std::move(proj2))
                   .in2
            == last2;
    }
};

inline constexpr __starts_with_fn starts_with = {};

}
