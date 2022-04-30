#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/type_traits.hpp>

#include <array>
#include <numeric>
#include <variant>

namespace iris::ranges {
namespace __concat_view_detail {
    template <typename... Ts>
    concept __have_common_reference = requires
    {
        typename std::common_reference_t<Ts...>;
    }
    &&(std::convertible_to<Ts, std::common_reference_t<Ts...>>&&...);

    template <typename... Ts>
    concept __have_common_type = requires
    {
        typename std::common_type_t<Ts...>;
    };

    // clang-format off
    template <typename... Ranges>
    concept __concat_indirectly_readable = 
        std::common_reference_with<
            std::common_reference_t<
                std::ranges::range_reference_t<Ranges>...>&&,
            std::common_type_t<
                std::ranges::range_value_t<Ranges>...>&>&&
        std::common_reference_with<
            std::common_reference_t<
                std::ranges::range_reference_t<Ranges>...>&&,
            std::common_reference_t<
                std::ranges::range_rvalue_reference_t<Ranges>...>&&>&&
        std::common_reference_with<
            std::common_reference_t<
                std::ranges::range_rvalue_reference_t<Ranges>...>&&,
            const std::common_type_t<
                std::ranges::range_value_t<Ranges>...>&>;
    // clang-format on

    // clang-format off
    template <typename... Ranges>
    concept __concatable = 
        __have_common_reference<std::ranges::range_reference_t<Ranges>...> 
        && __have_common_type<std::ranges::range_value_t<Ranges>...> 
        && __have_common_reference<std::ranges::range_rvalue_reference_t<Ranges>...> 
        && __concat_indirectly_readable<Ranges...>;
    // clang-format on

    template <typename... Ranges>
    concept __concat_random_access
        = ((std::ranges::random_access_range<
                Ranges> && std::ranges::sized_range<Ranges>)&&...);

    template <class Range>
    concept __constant_time_reversible
        = (std::ranges::bidirectional_range<
               Range> && std::ranges::common_range<Range>)
        || (std::ranges::sized_range<
                Range> && std::ranges::random_access_range<Range>);

    template <typename... Ranges>
    concept __concat_bidirectional
        = (std::ranges::bidirectional_range<Ranges> && ...)
        && __constant_time_reversible<back_of_t<void, Ranges...>>;

    template <std::size_t N, typename F, typename Variant>
    constexpr auto __visit(F&& f, Variant&& v)
    {
        if (v.index() == N) {
            return std::invoke(std::forward<F>(f),
                               std::integral_constant<std::size_t, N> {},
                               std::forward<Variant>(v));
        }
        if constexpr (N > 0) {
            return __visit<N - 1>(std::forward<F>(f), std::forward<Variant>(v));
        } else {
            return std::invoke(std::forward<F>(f),
                               std::integral_constant<std::size_t, N> {},
                               std::forward<Variant>(v));
        }
    }
}

template <std::ranges::input_range... Views>
    requires((std::ranges::view<Views> && ...) && (sizeof...(Views) > 0)
             && __concat_view_detail::__concatable<Views...>)
class concat_view : public std::ranges::view_interface<concat_view<Views...>> {
public:
    template <typename... Bases>
    class iterator_base {
    };

    template <typename... Bases>
        requires(std::ranges::forward_range<Bases>&&...)
    class iterator_base<Bases...> {
    public:
        // clang-format off
        using iterator_category = 
            std::conditional_t<
                !std::is_lvalue_reference_v<
                    std::common_reference_t<std::ranges::range_reference_t<Bases>...>>,
            std::input_iterator_tag,
            std::conditional_t<
                (std::derived_from<
                    typename std::iterator_traits<
                        std::ranges::iterator_t<Bases>>::iterator_category,
                    std::random_access_iterator_tag> && ...) 
                && __concat_view_detail::__concat_random_access<Bases...>,
            std::random_access_iterator_tag,
            std::conditional_t<
                (std::derived_from<
                    typename std::iterator_traits<
                        std::ranges::iterator_t<Bases>>::iterator_category,
                    std::bidirectional_iterator_tag> && ...) 
                && __concat_view_detail::__concat_bidirectional<Bases...>,
            std::bidirectional_iterator_tag,
            std::conditional_t<
                (std::derived_from<
                    typename std::iterator_traits<
                        std::ranges::iterator_t<Bases>>::iterator_category,
                    std::forward_iterator_tag> && ...) ,
            std::forward_iterator_tag,
            std::input_iterator_tag>>>>;
        // clang-format on
    };

    template <bool IsConst>
    class iterator
        : public iterator_base<__detail::__maybe_const<IsConst, Views>...> {
        friend class concat_view;

        using Parent = __detail::__maybe_const<IsConst, concat_view>;
        using BaseIterator = std::variant<std::ranges::iterator_t<
            __detail::__maybe_const<IsConst, Views>>...>;

    public:
        // clang-format off
        using iterator_concept = 
            std::conditional_t<
                __concat_view_detail::__concat_random_access<
                    __detail::__maybe_const<IsConst, Views>...>,
            std::random_access_iterator_tag,
            std::conditional_t<
                __concat_view_detail::__concat_bidirectional<
                    __detail::__maybe_const<IsConst, Views>...>,
            std::bidirectional_iterator_tag,
            std::conditional_t<
                (std::ranges::forward_range<__detail::__maybe_const<IsConst, Views>> && ...),
            std::forward_iterator_tag,
            std::input_iterator_tag>>>;
        // clang-format on
        using value_type = std::common_type_t<std::ranges::range_value_t<
            __detail::__maybe_const<IsConst, Views>>...>;
        using difference_type
            = std::common_type_t<std::ranges::range_difference_t<
                __detail::__maybe_const<IsConst, Views>>...>;

        iterator() = default;

        constexpr iterator(iterator<!IsConst> other) requires(
            IsConst
            && (std::convertible_to<
                    std::ranges::iterator_t<Views>,
                    std::ranges::iterator_t<
                        __detail::__maybe_const<IsConst, Views>>> && ...))
            : parent_(other.parent_)
            , it_(std::move(other.it_))
        {
        }

        constexpr decltype(auto) operator*() const
        {
            IRIS_ASSERT(!it_.valueless_by_exception());

            using reference
                = std::common_reference_t<std::ranges::range_reference_t<
                    __detail::__maybe_const<IsConst, Views>>...>;
            return std::visit([](auto&& it) -> reference { return *it; }, it_);
        }

        constexpr iterator& operator++()
        {
            IRIS_ASSERT(!it_.valueless_by_exception());
            __concat_view_detail::__visit<sizeof...(Views) - 1>(
                [&](auto I, auto& it) {
                    ++std::get<I>(it_);
                    satisfy<I>();
                },
                it_);
            return *this;
        }

        constexpr decltype(auto) operator++(int)
        {
            if constexpr ((std::ranges::forward_range<__detail::__maybe_const<
                               IsConst, Views>> && ...)) {
                auto tmp = *this;
                ++*this;
                return tmp;
            } else {
                ++*this;
            }
        }

        constexpr iterator& operator--() //
            requires __concat_view_detail::__concat_bidirectional<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!it_.valueless_by_exception());
            __concat_view_detail::__visit<sizeof...(Views) - 1>(
                [&](auto I, auto& it) { prev<I>(); }, it_);
            return *this;
        }

        constexpr iterator operator--(int) //
            requires __concat_view_detail::__concat_bidirectional<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!it_.valueless_by_exception());
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr iterator& operator+=(difference_type offset) requires
            __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!it_.valueless_by_exception());
            if (offset > 0) {
                __concat_view_detail::__visit<sizeof...(Views) - 1>(
                    [&](auto I, auto& it) {
                        advance_fwd<I>(std::get<I>(it)
                                           - std::ranges::begin(
                                               std::get<I>(__parent_bases())),
                                       offset);
                    },
                    it_);
            } else if (offset < 0) {
                __concat_view_detail::__visit<sizeof...(Views) - 1>(
                    [&](auto I, auto& it) {
                        advance_bwd<I>(std::get<I>(it)
                                           - std::ranges::begin(
                                               std::get<I>(__parent_bases())),
                                       -offset);
                    },
                    it_);
            }
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!it_.valueless_by_exception());
            *this += -offset;
            return *this;
        }

        constexpr decltype(auto) operator[](difference_type offset) const //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            return *((*this) + offset);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs) //
            requires(std::equality_comparable<std::ranges::iterator_t<
                         __detail::__maybe_const<IsConst, Views>>>&&...)
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            return lhs.it_ == rhs.it_;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         std::default_sentinel_t)
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            constexpr auto last_idx = sizeof...(Views) - 1;
            return lhs.it_.index() == last_idx
                && std::get<last_idx>(lhs.it_)
                == std::ranges::end(std::get<last_idx>(lhs.__parent_bases()));
        }

        friend constexpr bool operator<(const iterator& lhs,
                                        const iterator& rhs) //
            requires(__concat_view_detail::__concat_random_access<
                     __detail::__maybe_const<IsConst, Views>>&&...)
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            return lhs.it_ < rhs.it_;
        }

        friend constexpr bool operator>(const iterator& lhs,
                                        const iterator& rhs) //
            requires(__concat_view_detail::__concat_random_access<
                     __detail::__maybe_const<IsConst, Views>>&&...)
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            return rhs < lhs;
        }

        friend constexpr bool operator<=(const iterator& lhs,
                                         const iterator& rhs) //
            requires(__concat_view_detail::__concat_random_access<
                     __detail::__maybe_const<IsConst, Views>>&&...)
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            return !(rhs < lhs);
        }

        friend constexpr bool operator>=(const iterator& lhs,
                                         const iterator& rhs) //
            requires(__concat_view_detail::__concat_random_access<
                     __detail::__maybe_const<IsConst, Views>>&&...)
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            return !(lhs < rhs);
        }

        friend constexpr auto operator<=>(const iterator& lhs,
                                          const iterator& rhs) //
            requires((
                __concat_view_detail::__concat_random_access<__detail::__maybe_const<
                    IsConst,
                    Views>> && std::three_way_comparable<__detail::__maybe_const<IsConst, Views>>)&&...)
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            return lhs.it_ <=> rhs.it_;
        }

        friend constexpr iterator operator+(const iterator& it,
                                            difference_type n) //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!it.it_.valueless_by_exception());
            return iterator { it } += n;
        }

        friend constexpr iterator operator+(difference_type n,
                                            const iterator& it) //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!it.it_.valueless_by_exception());
            return it + n;
        }

        friend constexpr iterator operator-(const iterator& it,
                                            difference_type n) //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!it.it_.valueless_by_exception());
            return iterator { it } -= n;
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   const iterator& rhs) //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            const auto il = lhs.it_.index();
            const auto ir = rhs.it_.index();
            if (il > ir) {
                auto dr = __concat_view_detail::__visit<sizeof...(Views) - 1>(
                    [&](auto I, auto& it) {
                        return std::ranges::end(
                                   std::get<I>(rhs.__parent_bases()))
                            - std::get<I>(it);
                    },
                    rhs.it_);
                auto dl = __concat_view_detail::__visit<sizeof...(Views) - 1>(
                    [&](auto I, auto& it) {
                        return std::get<I>(it)
                            - std::ranges::begin(
                                   std::get<I>(lhs.__parent_bases()));
                    },
                    lhs.it_);
                auto sizes = std::apply(
                    [](auto&... bases) {
                        return std::array { static_cast<difference_type>(
                            std::ranges::size(bases))... };
                    },
                    lhs.__parent_bases());
                auto s
                    = std::accumulate(sizes.data() + ir + 1, sizes.data() + il,
                                      difference_type(0), std::plus());
                return dr + s + dl;
            } else if (il < ir) {
                return -(rhs - lhs);
            } else {
                return __concat_view_detail::__visit<sizeof...(Views) - 1>(
                    [&](auto I, auto& it) {
                        return std::get<I>(it) - std::get<I>(rhs.it_);
                    },
                    lhs.it_);
            }
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   std::default_sentinel_t) //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            // TODO:
        }

        friend constexpr difference_type operator-(std::default_sentinel_t lhs,
                                                   const iterator& rhs) //
            requires __concat_view_detail::__concat_random_access<
                __detail::__maybe_const<IsConst, Views>...>
        {
            return -(rhs - lhs);
        }

        // clang-format off
        friend constexpr decltype(auto) iter_move(const iterator & it) noexcept(
            noexcept(((
                std::is_nothrow_invocable_v<
                    decltype(std::ranges::iter_move),
                    const std::ranges::iterator_t<
                        __detail::__maybe_const<IsConst, Views>>&> 
                && std::is_nothrow_convertible_v<
                    std::ranges::range_rvalue_reference_t<
                        __detail::__maybe_const<IsConst, Views>>, 
                    std::common_reference_t<
                        std::ranges::range_rvalue_reference_t<
                            __detail::__maybe_const<IsConst, Views>>...>>) 
                && ...)))
        // clang-format on
        {
            IRIS_ASSERT(!it.it_.valueless_by_exception());
            return std::visit(
                [](auto const& i)
                    -> std::common_reference_t<
                        std::ranges::range_rvalue_reference_t<
                            __detail::__maybe_const<IsConst, Views>>...> {
                    return std::ranges::iter_move(i);
                },
                it.it_);
        }

        friend constexpr void iter_swap(const iterator& lhs,
                                        const iterator& rhs) //
            noexcept(noexcept(
                std::visit(std::ranges::iter_swap, lhs.it_, rhs.it_))) //
            requires requires
        {
            std::visit(std::ranges::iter_swap, lhs.it_, rhs.it_);
        }
        {
            IRIS_ASSERT(!lhs.it_.valueless_by_exception());
            IRIS_ASSERT(!rhs.it_.valueless_by_exception());
            std::visit(std::ranges::iter_swap, lhs.it_, rhs.it_);
        }

    private:
        template <typename... Args>
            requires std::constructible_from<BaseIterator, Args&&...>
        constexpr explicit iterator(Parent& parent, Args&&... args)
            : parent_(std::addressof(parent))
            , it_(std::forward<Args>(args)...)
        {
        }

        template <std::size_t N>
        constexpr void satisfy()
        {
            if constexpr (N != (sizeof...(Views) - 1)) {
                if (std::get<N>(it_)
                    == std::ranges::end(std::get<N>(parent_->bases_))) {
                    it_.template emplace<N + 1>(
                        std::ranges::begin(std::get<N + 1>(parent_->bases_)));
                    satisfy<N + 1>();
                }
            }
        }

        template <std::size_t N>
        constexpr void prev()
        {
            if constexpr (N == 0) {
                --std::get<0>(it_);
            } else {
                if (std::get<N>(it_)
                    == std::ranges::begin(std::get<N>(parent_->bases_))) {
                    using prev_view = __detail::__maybe_const<
                        IsConst,
                        std::tuple_element_t<N - 1, std::tuple<Views...>>>;
                    if constexpr (std::ranges::common_range<prev_view>) {
                        it_.template emplace<N - 1>(
                            std::ranges::end(get<N - 1>(parent_->bases_)));
                    } else {
                        it_.template emplace<N - 1>(std::ranges::next(
                            std::ranges::begin(
                                std::get<N - 1>(parent_->bases_)),
                            std::ranges::size(
                                std::get<N - 1>(__parent_bases()))));
                    }
                    prev<N - 1>();
                } else {
                    --std::get<N>(it_);
                }
            }
        }

        template <std::size_t N>
        constexpr void advance_fwd(difference_type offset,
                                   difference_type steps)
        {
            if constexpr (N == sizeof...(Views) - 1) {
                std::get<N>(it_) += steps;
            } else {
                auto n_size = std::ranges::size(std::get<N>(__parent_bases()));
                if (offset + steps < static_cast<difference_type>(n_size)) {
                    std::get<N>(it_) += steps;
                } else {
                    it_.template emplace<N + 1>(
                        std::ranges::begin(get<N + 1>(__parent_bases())));
                    advance_fwd<N + 1>(0, offset + steps - n_size);
                }
            }
        }

        template <std::size_t N>
        constexpr void advance_bwd(difference_type offset,
                                   difference_type steps)
        {
            if constexpr (N == 0) {
                std::get<N>(it_) -= steps;
            } else {
                if (offset >= steps) {
                    std::get<N>(it_) -= steps;
                } else {
                    it_.template emplace<N - 1>(
                        std::ranges::begin(get<N - 1>(__parent_bases()))
                        + std::ranges::size(get<N - 1>(__parent_bases())));
                    advance_bwd<N - 1>(
                        static_cast<difference_type>(
                            std::ranges::size(get<N - 1>(__parent_bases()))),
                        steps - offset);
                }
            }
        }

        constexpr auto& __parent_bases() const
        {
            return parent_->bases_;
        }

        Parent* parent_ {};
        BaseIterator it_ {};
    };

    constexpr concat_view() requires(std::default_initializable<Views>&&...)
        = default;

    constexpr explicit concat_view(Views... bases)
        : bases_(std::move(bases)...)
    {
    }

    constexpr auto begin() //
        requires(!(__detail::__simple_view<Views> && ...))
    {
        iterator<false> it(*this, std::in_place_index<0>,
                           std::ranges::begin(std::get<0>(bases_)));
        it.template satisfy<0>();
        return it;
    }

    constexpr auto begin() const
        requires((std::ranges::range<const Views> && ...)
                 && __concat_view_detail::__concatable<const Views...>)
    {
        iterator<true> it(*this, std::in_place_index<0>,
                          std::ranges::begin(std::get<0>(bases_)));
        it.template satisfy<0>();
        return it;
    }

    constexpr auto end() requires(!(__detail::__simple_view<Views> && ...))
    {
        if constexpr (std::ranges::common_range<back_of_t<void, Views...>>) {
            constexpr auto N = sizeof...(Views);
            return iterator<false>(*this, std::in_place_index<N - 1>,
                                   std::ranges::end(get<N - 1>(bases_)));
        } else {
            return std::default_sentinel;
        }
    }

    constexpr auto end() const requires(std::ranges::range<const Views>&&...)
    {
        if constexpr (std::ranges::common_range<back_of_t<void, Views...>>) {
            constexpr auto N = sizeof...(Views);
            return iterator<true>(this, std::in_place_index<N - 1>,
                                  std::ranges::end(get<N - 1>(bases_)));
        } else {
            return std::default_sentinel;
        }
    }

    constexpr auto size() requires(std::ranges::sized_range<Views>&&...)
    {
        return std::apply(
            [](auto... sizes) {
                using CT = std::make_unsigned_t<
                    std::common_type_t<decltype(sizes)...>>;
                return (CT { 0 } + ... + CT { sizes });
            },
            __detail::__tuple_transform(std::ranges::size, bases_));
    }

    constexpr auto size() const
        requires(std::ranges::sized_range<const Views>&&...)
    {
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    std::tuple<Views...> bases_ {};
};

template <typename... Ranges>
concat_view(Ranges&&...) -> concat_view<std::views::all_t<Ranges>...>;

namespace views {
    class __concat_fn {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range range) const
        {
            return std::views::all(std::forward<Range>(range));
        }

        template <std::ranges::viewable_range... Ranges>
            requires(sizeof...(Ranges) > 1)
        constexpr auto operator()(Ranges&&... ranges) const
        {
            return concat_view(std::forward<Ranges>(ranges)...);
        }
    };

    inline constexpr __concat_fn concat {};
}
}

namespace iris {
namespace views = ranges::views;
}
