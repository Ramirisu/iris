#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>

#include <array>

namespace iris::ranges {
namespace __cartesian_product_view_detail {
    template <typename First, typename... Rests>
    concept __cartesian_product_is_random_access
        = (std::ranges::random_access_range<First> && ...
           && (std::ranges::random_access_range<
                   Rests> && std::ranges::sized_range<Rests>));

    template <typename Range>
    concept __cartesian_product_common_arg
        = std::ranges::common_range<Range> ||(
            std::ranges::sized_range<Range>&&
                std::ranges::random_access_range<Range>);

    template <typename First, typename... Rests>
    concept __cartesian_product_is_bidrectional
        = (std::ranges::bidirectional_range<First> && ...
           && (std::ranges::bidirectional_range<
                   Rests> && std::ranges::sized_range<Rests>));

    template <typename First, typename... Rests>
    concept __cartesian_product_is_common
        = __cartesian_product_common_arg<First>;

    template <typename... Ranges>
    concept __cartesian_product_is_sized
        = (std::ranges::sized_range<Ranges> && ...);

    template <typename First, typename... Rests>
    concept __cartesian_sentinel_is_sized
        = (std::sized_sentinel_for<std::ranges::sentinel_t<First>,
                                   std::ranges::iterator_t<First>> && ...
           && std::ranges::sized_range<Rests>);

    template <__cartesian_product_common_arg Range>
    auto __cartesian_common_arg_end(Range& range)
    {
        if constexpr (std::ranges::common_range<Range>) {
            return std::ranges::end(range);
        } else {
            return std::ranges::begin(range) + std::ranges::size(range);
        }
    }
}

template <std::ranges::input_range First, std::ranges::forward_range... Rests>
    requires(std::ranges::view<First>&&...&& std::ranges::view<Rests>)
class cartesian_product_view : public std::ranges::view_interface<
                                   cartesian_product_view<First, Rests...>> {
public:
    template <bool IsConst>
    class iterator {
        friend class cartesian_product_view;

        using Parent = __detail::__maybe_const<IsConst, cartesian_product_view>;

    public:
        using iterator_category = std::input_iterator_tag;
        // clang-format off
        using iterator_concept = std::conditional_t<
            __cartesian_product_view_detail::__cartesian_product_is_random_access<First, Rests...>, 
            std::random_access_iterator_tag,
            std::conditional_t<
            __cartesian_product_view_detail::__cartesian_product_is_bidrectional<First, Rests...>, 
            std::bidirectional_iterator_tag, 
            std::conditional_t<
            std::ranges::forward_range<First>, 
            std::forward_iterator_tag, 
            std::input_iterator_tag>>>;
        // clang-format on
        using value_type = __detail::__tuple_or_pair_t<
            std::ranges::range_value_t<__detail::__maybe_const<IsConst, First>>,
            std::ranges::range_value_t<
                __detail::__maybe_const<IsConst, Rests>>...>;
        using reference = __detail::__tuple_or_pair_t<
            std::ranges::range_reference_t<
                __detail::__maybe_const<IsConst, First>>,
            std::ranges::range_reference_t<
                __detail::__maybe_const<IsConst, Rests>>...>;
        using difference_type
            = std::common_type_t<std::ranges::range_difference_t<First>,
                                 std::ranges::range_difference_t<Rests>...>;

        iterator() = default;

        constexpr iterator(iterator<!IsConst> other) requires(
            IsConst&& std::convertible_to<
                std::ranges::iterator_t<First>,
                std::ranges::iterator_t<__detail::__maybe_const<
                    IsConst,
                    First>>> && (std::convertible_to<std::ranges::iterator_t<Rests>, std::ranges::iterator_t<__detail::__maybe_const<IsConst, Rests>>> && ...))
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
            next();
            return *this;
        }

        constexpr iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr iterator& operator--() //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_bidrectional<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            prev();
            return *this;
        }

        constexpr iterator operator--(int) //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_bidrectional<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr iterator& operator+=(difference_type offset) //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_random_access<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            if (offset > 0) {
                while (offset-- > 0) {
                    next();
                }
            } else if (offset < 0) {
                while (offset++ < 0) {
                    prev();
                }
            }
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_random_access<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            *this += -offset;
            return *this;
        }

        constexpr reference operator[](difference_type offset) const //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_random_access<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            return *((*this) + offset);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs) //
            requires(std::equality_comparable<std::ranges::iterator_t<
                         __detail::__maybe_const<IsConst, First>>>&&...&&
                         std::equality_comparable<std::ranges::iterator_t<
                             __detail::__maybe_const<IsConst, Rests>>>)
        {
            return lhs.current_ == rhs.current_;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const std::default_sentinel_t&)
        {
            return std::get<0>(lhs.current_)
                == std::ranges::end(std::get<0>(lhs.parent_->bases_));
        }

        friend constexpr auto operator<(const iterator& lhs,
                                        const iterator& rhs) //
            requires(std::ranges::random_access_range<
                     __detail::__maybe_const<IsConst, First>>&&...&&
                         std::ranges::random_access_range<
                             __detail::__maybe_const<IsConst, Rests>>)
        {
            return lhs.current_ < rhs.current_;
        }

        friend constexpr auto operator>(const iterator& lhs,
                                        const iterator& rhs) //
            requires(std::ranges::random_access_range<
                     __detail::__maybe_const<IsConst, First>>&&...&&
                         std::ranges::random_access_range<
                             __detail::__maybe_const<IsConst, Rests>>)
        {
            return rhs < lhs;
        }

        friend constexpr auto operator<=(const iterator& lhs,
                                         const iterator& rhs) //
            requires(std::ranges::random_access_range<
                     __detail::__maybe_const<IsConst, First>>&&...&&
                         std::ranges::random_access_range<
                             __detail::__maybe_const<IsConst, Rests>>)
        {
            return !(rhs < lhs);
        }

        friend constexpr auto operator>=(const iterator& lhs,
                                         const iterator& rhs) //
            requires(std::ranges::random_access_range<
                     __detail::__maybe_const<IsConst, First>>&&...&&
                         std::ranges::random_access_range<
                             __detail::__maybe_const<IsConst, Rests>>)
        {
            return !(lhs < rhs);
        }

        friend constexpr auto operator<=>(const iterator& lhs,
                                          const iterator& rhs) //
            requires((
                std::ranges::random_access_range<
                    __detail::__maybe_const<IsConst, First>> && ...
                && std::ranges::random_access_range<__detail::__maybe_const<
                    IsConst,
                    Rests>>)&&(std::
                                   three_way_comparable<std::ranges::iterator_t<
                                       __detail::__maybe_const<IsConst,
                                                               First>>>&&...&&
                                       std::three_way_comparable<
                                           std::ranges::iterator_t<
                                               __detail::__maybe_const<
                                                   IsConst,
                                                   Rests>>>))
        {
            return lhs.current_ <=> rhs.current_;
        }

        friend constexpr iterator operator+(const iterator& i,
                                            difference_type n) //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_random_access<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            auto r = i;
            r += n;
            return r;
        }

        friend constexpr iterator operator+(difference_type n,
                                            const iterator& i) //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_random_access<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            auto r = i;
            r += n;
            return r;
        }

        friend constexpr iterator operator-(const iterator& i,
                                            difference_type n) //
            requires(__cartesian_product_view_detail::
                         __cartesian_product_is_random_access<
                             __detail::__maybe_const<IsConst, First>,
                             __detail::__maybe_const<IsConst, Rests>...>)
        {
            auto r = i;
            r -= n;
            return r;
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   const iterator& rhs) //
            requires(std::sized_sentinel_for<
                     std::ranges::iterator_t<
                         __detail::__maybe_const<IsConst, First>>,
                     std::ranges::iterator_t<
                         __detail::__maybe_const<IsConst, First>>>&&...&&
                         std::sized_sentinel_for<
                             std::ranges::iterator_t<
                                 __detail::__maybe_const<IsConst, Rests>>,
                             std::ranges::iterator_t<
                                 __detail::__maybe_const<IsConst, Rests>>>)
        {
            return lhs.distance_to(rhs.current_);
        }

        friend constexpr difference_type operator-(iterator i,
                                                   std::default_sentinel_t) //
            requires
            __cartesian_product_view_detail::__cartesian_product_is_sized<
                Rests...>
        {
            return i.distance_to(std::apply(
                [](auto&& first, auto&&... rests) {
                    return std::tuple { std::ranges::end(first),
                                        std::ranges::begin(rests)... };
                },
                i.parent_->base_));
        }

        friend constexpr difference_type operator-(std::default_sentinel_t s,
                                                   iterator i) //
            requires
            __cartesian_product_view_detail::__cartesian_product_is_sized<
                Rests...>
        {
            return -(i - s);
        }

        friend constexpr auto iter_move(const iterator& i) noexcept(
            (noexcept(std::ranges::iter_move(
                 std::declval<const std::ranges::iterator_t<
                     __detail::__maybe_const<IsConst, First>>&>()))
             && ...&& noexcept(std::ranges::iter_move(
                 std::declval<const std::ranges::iterator_t<
                     __detail::__maybe_const<IsConst, Rests>>&>())))
            && (std::is_nothrow_move_constructible_v<
                    std::ranges::range_rvalue_reference_t<
                        __detail::__maybe_const<IsConst, First>>> && ...
                && std::is_nothrow_move_constructible_v<
                    std::ranges::range_rvalue_reference_t<
                        __detail::__maybe_const<IsConst, Rests>>>))
        {
            return __detail::__tuple_transform(std::ranges::iter_move,
                                               i.current_);
        }

        friend constexpr void
        iter_swap(const iterator& lhs, const iterator& rhs) noexcept(
            noexcept(__detail::__tuple_iter_swap(
                lhs.current_,
                rhs.current_,
                std::index_sequence_for<First, Rests...> {}))) //
            requires(std::indirectly_swappable<std::ranges::iterator_t<
                         __detail::__maybe_const<IsConst, First>>>&&...&&
                         std::indirectly_swappable<std::ranges::iterator_t<
                             __detail::__maybe_const<IsConst, Rests>>>)
        {
            __detail::__tuple_iter_swap(
                lhs.current_, rhs.current_,
                std::index_sequence_for<First, Rests...> {});
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit iterator(
            Parent& parent,
            __detail::__tuple_or_pair_t<
                std::ranges::iterator_t<
                    __detail::__maybe_const<IsConst, First>>,
                std::ranges::iterator_t<
                    __detail::__maybe_const<IsConst, Rests>>...> current)
            : parent_(std::addressof(parent))
            , current_(std::move(current))
        {
        }

        template <std::size_t N = sizeof...(Rests)>
        void next()
        {
            auto& it = std::get<N>(current_);
            ++it;
            if constexpr (N > 0) {
                if (it == std::ranges::end(std::get<N>(parent_->bases_))) {
                    it = std::ranges::begin(std::get<N>(parent_->bases_));
                    next<N - 1>();
                }
            }
        }

        template <std::size_t N = sizeof...(Rests)>
        void prev()
        {
            auto& it = std::get<N>(current_);
            if (it == std::ranges::begin(std::get<N>(parent_->bases_))) {
                std::ranges::advance(
                    it,
                    __cartesian_product_view_detail::__cartesian_common_arg_end(
                        std::get<N>(parent_->bases_)));
                if constexpr (N > 0) {
                    prev<N - 1>();
                }
            }
            --it;
        }

        template <class Tuple>
        difference_type distance_to(Tuple t) const
        {
            auto sizes = std::apply(
                [](auto&&... args) {
                    return std::array { static_cast<difference_type>(
                                            std::ranges::size(args))...,
                                        difference_type(1) };
                },
                parent_->bases_);

            for (auto i = sizes.size() - 1; i > 0; --i) {
                sizes[i - 1] *= sizes[i];
            }

            return [&]<std::size_t... Is>(auto& current, Tuple& end,
                                          std::index_sequence<Is...>)
            {
                return ((static_cast<difference_type>(std::get<Is>(current)
                                                      - std::get<Is>(end))
                         * sizes[Is + 1])
                        + ...);
            }
            (current_, t, std::index_sequence_for<First, Rests...> {});
        }

        Parent* parent_ {};
        __detail::__tuple_or_pair_t<
            std::ranges::iterator_t<__detail::__maybe_const<IsConst, First>>,
            std::ranges::iterator_t<__detail::__maybe_const<IsConst, Rests>>...>
            current_ {};
    };

    // clang-format off
    cartesian_product_view() 
        requires std::default_initializable<First> 
            && (std::default_initializable<Rests> && ...) = default;
    // clang-format on

    constexpr explicit cartesian_product_view(First first, Rests... rests)
        : bases_(std::move(first), std::move(rests)...)
    {
    }

    constexpr auto begin() //
        requires(!__detail::__simple_view<First> || ...
                 || !__detail::__simple_view<Rests>)
    {
        return iterator<false>(
            *this, __detail::__tuple_transform(std::ranges::begin, bases_));
    }

    constexpr auto begin() const //
        requires(std::ranges::range<const First>&&...&&
                     std::ranges::range<const Rests>)
    {
        return iterator<true>(
            *this, __detail::__tuple_transform(std::ranges::begin, bases_));
    }

    constexpr auto end() //
        requires((!__detail::__simple_view<First> || ...
                  || !__detail::__simple_view<
                      Rests>)&&__cartesian_product_view_detail::
                     __cartesian_product_is_common<First, Rests...>)
    {
        iterator<false> it(
            *this, __detail::__tuple_transform(std::ranges::begin, bases_));
        std::get<0>(it.current_)
            = __cartesian_product_view_detail::__cartesian_common_arg_end(
                std::get<0>(bases_));
        return it;
    }

    constexpr auto end() const //
        requires(__cartesian_product_view_detail::
                     __cartesian_product_is_common<const First, const Rests...>)
    {
        iterator<true> it(
            *this, __detail::__tuple_transform(std::ranges::begin, bases_));
        std::get<0>(it.current_)
            = __cartesian_product_view_detail::__cartesian_common_arg_end(
                std::get<0>(bases_));
        return it;
    }

    constexpr std::default_sentinel_t end() const //
        requires(!__cartesian_product_view_detail::
                     __cartesian_product_is_common<const First, const Rests...>)
    {
        return {};
    }

    constexpr auto size() //
        requires
        __cartesian_product_view_detail::__cartesian_product_is_sized<First,
                                                                      Rests...>
    {
        const auto value = std::apply(
            [&](auto&&... bases) {
                using common_type
                    = std::common_type_t<std::ranges::range_size_t<First>,
                                         std::ranges::range_size_t<Rests>...>;
                return std::max(
                    { static_cast<common_type>(std::ranges::size(bases))... });
            },
            bases_);
        return static_cast<std::make_unsigned_t<decltype(value)>>(value);
    }

    constexpr auto size() const //
        requires __cartesian_product_view_detail::
            __cartesian_product_is_sized<const First, const Rests...>
    {
        const auto value = std::apply(
            [&](auto&&... bases) {
                using common_type
                    = std::common_type_t<std::ranges::range_size_t<First>,
                                         std::ranges::range_size_t<Rests>...>;
                return std::max(
                    { static_cast<common_type>(std::ranges::size(bases))... });
            },
            bases_);
        return static_cast<std::make_unsigned_t<decltype(value)>>(value);
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    std::tuple<First, Rests...> bases_ {};
};

template <typename... Ranges>
cartesian_product_view(Ranges&&...)
    -> cartesian_product_view<std::views::all_t<Ranges>...>;

namespace views {
    class __cartesian_product_fn {
    public:
        constexpr __cartesian_product_fn() noexcept = default;

        constexpr auto operator()() const
        {
            return std::views::empty<std::tuple<>>;
        }

        template <std::ranges::viewable_range... Ranges>
            requires(sizeof...(Ranges) > 0)
        constexpr auto operator()(Ranges&&... ranges) const
        {
            return cartesian_product_view<std::views::all_t<Ranges>...> {
                std::forward<Ranges>(ranges)...
            };
        }
    };

    inline constexpr __cartesian_product_fn cartesian_product {};
}
}

namespace iris {
namespace views = ranges::views;
}
