#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {
namespace __fold_detail {
    template <class F>
    struct __flipped {
        F f;

        // clang-format off
        template <class T, class U>
            requires std::invocable<F&, U, T> 
        std::invoke_result_t<F&, U, T> operator()(T&&, U&&);
        // clang-format on
    };

    // clang-format off
    template <class F, class T, class I, class U>
    concept __indirectly_binary_left_foldable_impl = 
        std::movable<T> 
        && std::movable<U> 
        && std::convertible_to<T, U> 
        && std::invocable<F&, U, std::iter_reference_t<I>> 
        && std::assignable_from<
            U&, 
            std::invoke_result_t<F&, U, std::iter_reference_t<I>>>;
    // clang-format on

    // clang-format off
    template <typename F, typename T, typename I>
    concept __indirectly_binary_left_foldable = 
        std::copy_constructible<F> 
        && std::indirectly_readable<I> 
        && std::invocable<F&, T, std::iter_reference_t<I>> 
        && std::convertible_to<
            std::invoke_result_t<F&, T, std::iter_reference_t<I>>,
            std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>> 
        && __indirectly_binary_left_foldable_impl<
            F, 
            T, 
            I, 
            std::decay_t<std::invoke_result_t<F&, T, std::iter_reference_t<I>>>>;
    // clang-format on

    template <class F, class T, class I>
    concept __indirectly_binary_right_foldable
        = __indirectly_binary_left_foldable<__flipped<F>, T, I>;
}

template <class I, class T>
struct in_value_result {
    [[no_unique_address]] I in;
    [[no_unique_address]] T value;
    template <class I2, class T2>
        requires std::convertible_to<const I&,
                                     I2> && std::convertible_to<const T&, T2>
    constexpr operator in_value_result<I2, T2>() const&
    {
        return { in, value };
    }
    template <class I2, class T2>
        requires std::convertible_to<I, I2> && std::convertible_to<T, T2>
    constexpr operator in_value_result<I2, T2>() &&
    {
        return { std::move(in), std::move(value) };
    }
};

template <class I, class T>
using fold_left_with_iter_result = in_value_result<I, T>;

template <class I, class T>
using fold_left_first_with_iter_result = in_value_result<I, T>;

struct __fold_left_with_iter_fn {
    template <std::input_iterator I,
              std::sentinel_for<I> S,
              class T,
              __fold_detail::__indirectly_binary_left_foldable<T, I> F>
    constexpr auto operator()(I first, S last, T init, F f) const
    {
        using U = std::decay_t<
            std::invoke_result_t<F&, T, std::iter_reference_t<I>>>;

        if (first == last) {
            return fold_left_with_iter_result<I, U> { std::move(first),
                                                      U(std::move(init)) };
        }

        U accum = std::invoke(f, std::move(init), *first);
        for (++first; first != last; ++first) {
            accum = std::invoke(f, std::move(accum), *first);
        }
        return fold_left_with_iter_result<I, U> { std::move(first),
                                                  std::move(accum) };
    }

    template <std::ranges::input_range R,
              class T,
              __fold_detail::__indirectly_binary_left_foldable<
                  T,
                  std::ranges::iterator_t<R>> F>
    constexpr auto operator()(R&& r, T init, F f) const
    {
        using U = std::decay_t<
            std::invoke_result_t<F&, T, std::ranges::range_reference_t<R>>>;
        return fold_left_with_iter_result<std::ranges::borrowed_iterator_t<R>,
                                          U>(
            (*this)(std::ranges::begin(std::forward<R>(r)),
                    std::ranges::end(std::forward<R>(r)), std::move(init), f));
    }
};

inline constexpr __fold_left_with_iter_fn fold_left_with_iter {};

struct __fold_left_fn {
    template <std::input_iterator I,
              std::sentinel_for<I> S,
              class T,
              __fold_detail::__indirectly_binary_left_foldable<T, I> F>
    constexpr auto operator()(I first, S last, T init, F f) const
    {
        return fold_left_with_iter(std::move(first), last, std::move(init), f)
            .value;
    }

    template <std::ranges::input_range R,
              class T,
              __fold_detail::__indirectly_binary_left_foldable<
                  T,
                  std::ranges::iterator_t<R>> F>
    constexpr auto operator()(R&& r, T init, F f) const
    {
        return fold_left_with_iter(std::forward<R>(r), std::move(init), f)
            .value;
    }
};

inline constexpr __fold_left_fn fold_left {};

struct __fold_left_first_with_iter_fn {
    template <std::input_iterator I,
              std::sentinel_for<I> S,
              __fold_detail::
                  __indirectly_binary_left_foldable<std::iter_value_t<I>, I> F>
        requires std::constructible_from<std::iter_value_t<I>,
                                         std::iter_reference_t<I>>
    constexpr auto operator()(I first, S last, F f) const
    {
        using U = decltype(fold_left(std::move(first), last,
                                     std::iter_value_t<I>(*first), f));

        if (first == last) {
            return fold_left_first_with_iter_result<I, std::optional<U>> {
                std::move(first), std::optional<U>()
            };
        }

        std::optional<U> init(std::in_place, *first);
        for (++first; first != last; ++first) {
            *init = std::invoke(f, std::move(*init), *first);
        }

        return fold_left_first_with_iter_result<I, std::optional<U>> {
            std::move(first), std::move(init)
        };
    }

    template <std::ranges::input_range R,
              __fold_detail::__indirectly_binary_left_foldable<
                  std::ranges::range_value_t<R>,
                  std::ranges::iterator_t<R>> F>
        requires std::constructible_from<std::ranges::range_value_t<R>,
                                         std::ranges::range_reference_t<R>>
    constexpr auto operator()(R&& r, F f) const
    {
        using U = decltype(fold_left(std::forward<R>(r),
                                     std::ranges::range_value_t<R>(), f));
        return fold_left_first_with_iter_result<
            std::ranges::borrowed_iterator_t<R>, std::optional<U>>(
            (*this)(std::ranges::begin(std::forward<R>(r)),
                    std::ranges::end(std::forward<R>(r)), f));
    }
};

inline constexpr __fold_left_first_with_iter_fn fold_left_first_with_iter {};

struct __fold_left_first_fn {
    template <std::input_iterator I,
              std::sentinel_for<I> S,
              __fold_detail::
                  __indirectly_binary_left_foldable<std::iter_value_t<I>, I> F>
        requires std::constructible_from<std::iter_value_t<I>,
                                         std::iter_reference_t<I>>
    constexpr auto operator()(I first, S last, F f) const
    {
        return fold_left_first_with_iter(std::move(first), last, f).value;
    }

    template <std::ranges::input_range R,
              __fold_detail::__indirectly_binary_left_foldable<
                  std::ranges::range_value_t<R>,
                  std::ranges::iterator_t<R>> F>
        requires std::constructible_from<std::ranges::range_value_t<R>,
                                         std::ranges::range_reference_t<R>>
    constexpr auto operator()(R&& r, F f) const
    {
        return fold_left_first_with_iter(std::forward<R>(r), f).value;
    }
};

inline constexpr __fold_left_first_fn fold_left_first {};

struct __fold_right_fn {
    template <std::bidirectional_iterator I,
              std::sentinel_for<I> S,
              class T,
              __fold_detail::__indirectly_binary_right_foldable<T, I> F>
    constexpr auto operator()(I first, S last, T init, F f) const
    {
        using U = std::decay_t<
            std::invoke_result_t<F&, std::iter_reference_t<I>, T>>;

        if (first == last) {
            return U(std::move(init));
        }

        I tail = std::ranges::next(first, last);
        U accum = std::invoke(f, *--tail, std::move(init));
        while (first != tail) {
            accum = std::invoke(f, *--tail, std::move(accum));
        }
        return accum;
    }

    template <std::ranges::bidirectional_range R,
              class T,
              __fold_detail::__indirectly_binary_right_foldable<
                  T,
                  std::ranges::iterator_t<R>> F>
    constexpr auto operator()(R&& r, T init, F f) const
    {
        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), init, f);
    }
};

inline constexpr __fold_right_fn fold_right {};

struct __fold_right_last_fn {
    template <std::bidirectional_iterator I,
              std::sentinel_for<I> S,
              __fold_detail::
                  __indirectly_binary_right_foldable<std::iter_value_t<I>, I> F>
        requires std::constructible_from<std::iter_value_t<I>,
                                         std::iter_reference_t<I>>
    constexpr auto operator()(I first, S last, F f) const
    {
        using U = decltype(fold_right(first, last, std::iter_value_t<I>(*first),
                                      f));

        if (first == last) {
            return std::optional<U>();
        }

        I tail = std::ranges::prev(std::ranges::next(first, std::move(last)));
        return std::optional<U>(std::in_place,
                                fold_right(std::move(first), tail,
                                           std::iter_value_t<I>(*tail),
                                           std::move(f)));
    }

    template <std::ranges::bidirectional_range R,
              __fold_detail::__indirectly_binary_right_foldable<
                  std::ranges::range_value_t<R>,
                  std::ranges::iterator_t<R>> F>
        requires std::constructible_from<std::ranges::range_value_t<R>,
                                         std::ranges::range_reference_t<R>>
    constexpr auto operator()(R&& r, F f) const
    {
        return (*this)(std::ranges::begin(std::forward<R>(r)),
                       std::ranges::end(std::forward<R>(r)), f);
    }
};

inline constexpr __fold_right_last_fn fold_right_last {};

}
