#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>
#include <iris/type_traits.hpp>

#include <algorithm>

namespace iris::ranges {
namespace __zip_view_detail {
    template <typename... Ranges>
    concept __zip_is_common = (pack_size_v<Ranges...> == 1
                               && (std::ranges::common_range<Ranges> && ...))
        || (!(std::ranges::bidirectional_range<Ranges> && ...)
            && (std::ranges::common_range<Ranges> && ...))
        || ((std::ranges::random_access_range<Ranges> && ...)
            && (std::ranges::sized_range<Ranges> && ...));

    template <bool Const, typename... Ranges>
    concept __all_random_access_range
        = (std::ranges::random_access_range<
               __detail::__maybe_const<Const, Ranges>> && ...);

    template <bool Const, typename... Ranges>
    concept __all_bidirectional_range
        = (std::ranges::bidirectional_range<
               __detail::__maybe_const<Const, Ranges>> && ...);

    template <bool Const, typename... Ranges>
    concept __all_forward_range
        = (std::ranges::forward_range<
               __detail::__maybe_const<Const, Ranges>> && ...);

    template <typename F, typename Tuple>
    constexpr void __tuple_for_each(F&& f, Tuple&& tuple)
    {
        std::apply(
            [&]<class... Ts>(Ts && ... elements) {
                (std::invoke(f, std::forward<Ts>(elements)), ...);
            },
            std::forward<Tuple>(tuple));
    }

    template <typename TupleLHS, typename TupleRHS, std::size_t... Is>
    constexpr bool __tuple_any_of_equal(TupleLHS&& lhs,
                                        TupleRHS&& rhs,
                                        std::index_sequence<Is...>)
    {
        return ((std::get<Is>(std::forward<TupleLHS>(lhs))
                 == std::get<Is>(std::forward<TupleRHS>(rhs)))
                || ...);
    }

    template <typename DiffType,
              typename TupleLHS,
              typename TupleRHS,
              std::size_t... Is>
    constexpr auto __tuple_smallest_dist(TupleLHS&& lhs,
                                         TupleRHS&& rhs,
                                         std::index_sequence<Is...>)
    {
        return std::ranges::min(
            { DiffType(std::get<Is>(std::forward<TupleLHS>(lhs))
                       - std::get<Is>(std::forward<TupleRHS>(rhs)))... },
            [](auto lhs, auto rhs) { return std::abs(lhs) < std::abs(rhs); });
    }
}

template <std::ranges::input_range... Views>
    requires((std::ranges::view<Views> && ...) && (pack_size_v<Views...> > 0))
class zip_view : public std::ranges::view_interface<zip_view<Views...>> {
public:
    template <bool Const>
    class iterator_base {
    };

    template <bool Const>
        requires __zip_view_detail::__all_forward_range<Const, Views...>
    class iterator_base<Const> {
    public:
        using iterator_category = std::input_iterator_tag;
    };

    template <bool Const>
    class iterator : public iterator_base<Const> {
        friend class zip_view;

    public:
        using iterator_concept = std::conditional_t<
            __zip_view_detail::__all_random_access_range<Const, Views...>,
            std::random_access_iterator_tag,
            std::conditional_t<
                __zip_view_detail::__all_bidirectional_range<Const, Views...>,
                std::bidirectional_iterator_tag,
                std::conditional_t<
                    __zip_view_detail::__all_forward_range<Const, Views...>,
                    std::forward_iterator_tag,
                    std::input_iterator_tag>>>;
        using value_type
            = __detail::__tuple_or_pair_t<std::ranges::range_value_t<
                __detail::__maybe_const<Const, Views>>...>;
        using difference_type
            = std::common_type_t<std::ranges::range_difference_t<
                __detail::__maybe_const<Const, Views>>...>;

        iterator() = default;

        constexpr iterator(iterator<!Const> other) requires Const
            &&(std::convertible_to<
                std::ranges::iterator_t<Views>,
                std::ranges::iterator_t<
                    __detail::__maybe_const<Const, Views>>>&&...)
            : current_(std::move(other.current_))
        {
        }

        constexpr auto operator*() const
        {
            return __detail::__tuple_transform(
                [](auto& i) -> decltype(auto) { return *i; }, current_);
        }

        constexpr iterator& operator++()
        {
            __zip_view_detail::__tuple_for_each([](auto& i) { ++i; }, current_);
            return *this;
        }

        constexpr decltype(auto) operator++(int)
        {
            if constexpr (__zip_view_detail::__all_forward_range<Const,
                                                                 Views...>) {
                auto tmp = *this;
                ++*this;
                return tmp;
            } else {
                ++*this;
            }
        }

        constexpr iterator& operator--() //
            requires
            __zip_view_detail::__all_bidirectional_range<Const, Views...>
        {
            __zip_view_detail::__tuple_for_each([](auto& i) { --i; }, current_);
            return *this;
        }

        constexpr iterator operator--(int) //
            requires
            __zip_view_detail::__all_bidirectional_range<Const, Views...>
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr iterator& operator+=(difference_type offset) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            __zip_view_detail::__tuple_for_each(
                [&]<class I>(I& i) { i += std::iter_difference_t<I>(offset); },
                current_);
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            __zip_view_detail::__tuple_for_each(
                [&]<class I>(I& i) { i -= std::iter_difference_t<I>(offset); },
                current_);
            return *this;
        }

        constexpr auto operator[](difference_type offset) const //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            return __detail::__tuple_transform(
                [&]<class I>(I& i) -> decltype(auto) {
                    return i[std::iter_difference_t<I>(offset)];
                },
                current_);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs) //
            requires(std::equality_comparable<std::ranges::iterator_t<
                         __detail::__maybe_const<Const, Views>>>&&...)
        {
            if constexpr (__zip_view_detail::__all_bidirectional_range<
                              Const, Views...>) {
                return lhs.current_ == rhs.current_;
            } else {
                return __zip_view_detail::__tuple_any_of_equal(
                    lhs.current_, rhs.current_,
                    std::index_sequence_for<Views...> {});
            }
        }

        friend constexpr bool operator<(const iterator& lhs,
                                        const iterator& rhs) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            return lhs.current_ < rhs.current_;
        }

        friend constexpr bool operator>(const iterator& lhs,
                                        const iterator& rhs) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            return rhs < lhs;
        }

        friend constexpr bool operator<=(const iterator& lhs,
                                         const iterator& rhs) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            return !(rhs < lhs);
        }

        friend constexpr bool operator>=(const iterator& lhs,
                                         const iterator& rhs) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            return !(lhs < rhs);
        }

        friend constexpr auto operator<=>(const iterator& lhs,
                                          const iterator& rhs) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...> &&(
                std::three_way_comparable<std::ranges::iterator_t<
                    __detail::__maybe_const<Const, Views>>>&&...)
        {
            return lhs.current_ <=> rhs.current_;
        }

        friend constexpr iterator operator+(const iterator& i,
                                            difference_type n) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            auto r = i;
            r += n;
            return r;
        }

        friend constexpr iterator operator+(difference_type n,
                                            const iterator& i) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            auto r = i;
            r += n;
            return r;
        }

        friend constexpr iterator operator-(const iterator& i,
                                            difference_type n) //
            requires
            __zip_view_detail::__all_random_access_range<Const, Views...>
        {
            auto r = i;
            r -= n;
            return r;
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   const iterator& rhs) //
            requires(std::sized_sentinel_for<
                     std::ranges::iterator_t<
                         __detail::__maybe_const<Const, Views>>,
                     std::ranges::iterator_t<
                         __detail::__maybe_const<Const, Views>>>&&...)
        {
            return __zip_view_detail::__tuple_smallest_dist<difference_type>(
                lhs.current_, rhs.current_,
                std::index_sequence_for<Views...> {});
        }

        friend constexpr auto iter_move(const iterator& i) //
            noexcept(
                (noexcept(std::ranges::iter_move(
                     std::declval<std::ranges::iterator_t<
                         __detail::__maybe_const<Const, Views>> const&>()))
                 && ...)
                && (std::is_nothrow_move_constructible_v<
                        std::ranges::range_rvalue_reference_t<
                            __detail::__maybe_const<Const, Views>>> && ...))
        {
            return __detail::__tuple_transform(std::ranges::iter_move,
                                               i.current_);
        }

        friend constexpr void iter_swap(const iterator& lhs,
                                        const iterator& rhs) //
            noexcept(noexcept(__detail::__tuple_iter_swap(
                lhs.current_,
                rhs.current_,
                std::index_sequence_for<Views...> {}))) //
            requires(std::indirectly_swappable<std::ranges::iterator_t<
                         __detail::__maybe_const<Const, Views>>>&&...)
        {
            __detail::__tuple_iter_swap(lhs.current_, rhs.current_,
                                        std::index_sequence_for<Views...> {});
        }

        constexpr auto& __current() const
        {
            return current_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit iterator(
            __detail::__tuple_or_pair_t<std::ranges::iterator_t<
                __detail::__maybe_const<Const, Views>>...> current)
            : current_(std::move(current))
        {
        }

        __detail::__tuple_or_pair_t<
            std::ranges::iterator_t<__detail::__maybe_const<Const, Views>>...>
            current_;
    };

    template <bool Const>
    class sentinel {
        friend class zip_view;

    public:
        sentinel() = default;

        constexpr sentinel(sentinel<!Const> other) //
            requires Const &&(std::convertible_to<
                              std::ranges::sentinel_t<Views>,
                              std::ranges::sentinel_t<
                                  __detail::__maybe_const<Const, Views>>>&&...)
            : end_(other.end_)
        {
        }

        template <bool OtherConst>
            requires(std::sentinel_for<
                     std::ranges::sentinel_t<
                         __detail::__maybe_const<OtherConst, Views>>,
                     std::ranges::iterator_t<
                         __detail::__maybe_const<OtherConst, Views>>>&&...)
        friend constexpr bool operator==(const iterator<OtherConst>& lhs,
                                         const sentinel& rhs)
        {
            return __zip_view_detail::__tuple_any_of_equal(
                lhs.__current(), rhs.end_,
                std::index_sequence_for<Views...> {});
        }

        template <bool OtherConst>
            requires(std::sized_sentinel_for<
                     std::ranges::sentinel_t<
                         __detail::__maybe_const<Const, Views>>,
                     std::ranges::iterator_t<
                         __detail::__maybe_const<OtherConst, Views>>>&&...)
        friend constexpr std::common_type_t<std::ranges::range_difference_t<
            __detail::__maybe_const<OtherConst, Views>>...>
        operator-(const iterator<OtherConst>& lhs, const sentinel& rhs)
        {
            return __zip_view_detail::__tuple_smallest_dist<
                std::common_type_t<std::ranges::range_difference_t<
                    __detail::__maybe_const<OtherConst, Views>>...>>(
                lhs.__current(), rhs.end_,
                std::index_sequence_for<Views...> {});
        }

        template <bool OtherConst>
            requires(std::sized_sentinel_for<
                     std::ranges::sentinel_t<
                         __detail::__maybe_const<Const, Views>>,
                     std::ranges::iterator_t<
                         __detail::__maybe_const<OtherConst, Views>>>&&...)
        friend constexpr std::common_type_t<std::ranges::range_difference_t<
            __detail::__maybe_const<OtherConst, Views>>...>
        operator-(const sentinel& lhs, const iterator<OtherConst>& rhs)
        {
            return -(rhs - lhs);
        }

    private:
        constexpr explicit sentinel(
            __detail::__tuple_or_pair_t<std::ranges::sentinel_t<
                __detail::__maybe_const<Const, Views>>...> end)
            : end_(end)
        {
        }

        __detail::__tuple_or_pair_t<
            std::ranges::sentinel_t<__detail::__maybe_const<Const, Views>>...>
            end_;
    };

    zip_view() = default;

    constexpr explicit zip_view(Views... bases)
        : bases_(std::move(bases)...)
    {
    }

    constexpr auto begin() requires(!(__detail::__simple_view<Views> && ...))
    {
        return iterator<false>(
            __detail::__tuple_transform(std::ranges::begin, bases_));
    }
    constexpr auto begin() const requires(std::ranges::range<const Views>&&...)
    {
        return iterator<true>(
            __detail::__tuple_transform(std::ranges::begin, bases_));
    }

    constexpr auto end() requires(!(__detail::__simple_view<Views> && ...))
    {
        if constexpr (!__zip_view_detail::__zip_is_common<Views...>) {
            return sentinel<false>(
                __detail::__tuple_transform(std::ranges::end, bases_));
        } else if constexpr ((std::ranges::random_access_range<Views> && ...)) {
            return begin() + iter_difference_t<iterator<false>>(size());
        } else {
            return iterator<false>(
                __detail::__tuple_transform(std::ranges::end, bases_));
        }
    }

    constexpr auto end() const requires(std::ranges::range<const Views>&&...)
    {
        if constexpr (!__zip_view_detail::__zip_is_common<const Views...>) {
            return sentinel<true>(
                __detail::__tuple_transform(std::ranges::end, bases_));
        } else if constexpr ((std::ranges::random_access_range<
                                  const Views> && ...)) {
            return begin() + std::iter_difference_t<iterator<true>>(size());
        } else {
            return iterator<true>(
                __detail::__tuple_transform(std::ranges::end, bases_));
        }
    }

    constexpr auto size() //
        requires(std::ranges::sized_range<Views>&&...)
    {
        return std::apply(
            [](auto... sizes) {
                using CT = std::make_unsigned_t<
                    std::common_type_t<decltype(sizes)...>>;
                return std::ranges::min({ CT(sizes)... });
            },
            __detail::__tuple_transform(std::ranges::size, bases_));
    }

    constexpr auto size() const //
        requires(std::ranges::sized_range<const Views>&&...)
    {
        return std::apply(
            [](auto... sizes) {
                using CT = std::make_unsigned_t<
                    std::common_type_t<decltype(sizes)...>>;
                return std::ranges::min({ CT(sizes)... });
            },
            __detail::__tuple_transform(std::ranges::size, bases_));
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    std::tuple<Views...> bases_;
};

template <typename... Ranges>
zip_view(Ranges&&...) -> zip_view<std::views::all_t<Ranges>...>;

namespace views {
    class __zip_fn : public range_adaptor_closure<__zip_fn> {
    public:
        constexpr auto operator()() const noexcept
        {
            return std::views::empty<std::tuple<>>;
        }

        template <std::ranges::viewable_range... Ranges>
            requires(pack_size_v<Ranges...> > 0)
        constexpr auto operator()(Ranges&&... ranges) const
            noexcept(noexcept(zip_view<std::views::all_t<Ranges&&>...>(
                std::forward<Ranges>(ranges)...)))
                -> decltype(zip_view<std::views::all_t<Ranges&&>...>(
                    std::forward<Ranges>(ranges)...))
        {
            return zip_view<std::views::all_t<Ranges&&>...>(
                std::forward<Ranges>(ranges)...);
        }
    };

    inline constexpr __zip_fn zip {};
}
}

namespace iris {
namespace views = ranges::views;
}

namespace std::ranges {
template <class... Views>
inline constexpr bool enable_borrowed_range<
    iris::ranges::zip_view<Views...>> = (enable_borrowed_range<Views> && ...);
}
