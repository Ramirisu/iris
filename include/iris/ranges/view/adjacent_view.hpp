#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

#include <array>

namespace iris::ranges {

template <std::ranges::forward_range View, std::size_t N>
    requires(std::ranges::view<View> && (N > 0))
class adjacent_view
    : public std::ranges::view_interface<adjacent_view<View, N>> {
    struct __as_sentinel_t {
    };

public:
    template <bool Const>
    class iterator {
        friend class adjacent_view;

        using Base = __detail::__maybe_const<Const, View>;

    public:
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::conditional_t<
            std::ranges::random_access_range<Base>,
            std::random_access_iterator_tag,
            std::conditional_t<std::ranges::bidirectional_range<Base>,
                               std::bidirectional_iterator_tag,
                               std::forward_iterator_tag>>;
        using value_type
            = __detail::__repeat_n_t<__detail::__tuple_or_pair_t,
                                     std::ranges::range_value_t<Base>,
                                     N>;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!Const> other) requires(
            Const&& std::convertible_to<std::ranges::iterator_t<View>,
                                        std::ranges::iterator_t<Base>>)
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
            for (std::size_t i = 0; i < N; ++i) {
                current_[i] = std::ranges::next(current_[i], 1);
            }

            return *this;
        }

        constexpr iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr iterator& operator--() //
            requires std::ranges::bidirectional_range<Base>
        {
            for (std::size_t i = 0; i < N; ++i) {
                current_[i] = std::ranges::prev(current_[i], 1);
            }

            return *this;
        }

        constexpr iterator operator--(int) //
            requires std::ranges::bidirectional_range<Base>
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr iterator& operator+=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            for (std::size_t i = 0; i < N; ++i) {
                current_[i] += offset;
            }

            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            for (std::size_t i = 0; i < N; ++i) {
                current_[i] -= offset;
            }

            return *this;
        }

        constexpr auto operator[](difference_type n) const //
            requires std::ranges::random_access_range<Base>
        {
            return __detail::__tuple_transform(
                [&](auto& i) -> decltype(auto) { return i[n]; }, current_);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.current_.back() == rhs.current_.back();
        }

        friend constexpr bool operator<(const iterator& lhs,
                                        const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return lhs.current_.back() < rhs.current_.back();
        }

        friend constexpr bool operator>(const iterator& lhs,
                                        const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return rhs < lhs;
        }

        friend constexpr bool operator<=(const iterator& lhs,
                                         const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return !(rhs < lhs);
        }

        friend constexpr bool operator>=(const iterator& lhs,
                                         const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return !(lhs < rhs);
        }

        friend constexpr auto operator<=>(const iterator& lhs,
                                          const iterator& rhs) //
            requires std::ranges::random_access_range<Base> && std::
                three_way_comparable<std::ranges::iterator_t<Base>>
        {
            return lhs.current_.back() <=> rhs.current_.back();
        }

        friend constexpr iterator operator+(const iterator& i,
                                            difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            auto r = i;
            r += offset;
            return r;
        }

        friend constexpr iterator operator+(difference_type offset,
                                            const iterator& i) //
            requires std::ranges::random_access_range<Base>
        {
            auto r = i;
            r += offset;
            return r;
        }

        friend constexpr iterator operator-(const iterator& i,
                                            difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            auto r = i;
            r -= offset;
            return r;
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   const iterator& rhs) //
            requires std::sized_sentinel_for<std::ranges::iterator_t<Base>,
                                             std::ranges::iterator_t<Base>>
        {
            return lhs.current_.back() - rhs.current_.back();
        }

        friend constexpr auto iter_move(const iterator& i) noexcept(
            noexcept(std::ranges::iter_move(
                std::declval<std::ranges::iterator_t<Base> const&>()))
            && std::is_nothrow_move_constructible_v<
                std::ranges::range_rvalue_reference_t<Base>>)
        {
            return __detail::__tuple_transform(std::ranges::iter_move,
                                               i.current_);
        }

        friend constexpr void
        iter_swap(const iterator& lhs, const iterator& rhs) noexcept(
            noexcept(std::ranges::iter_swap(
                std::declval<std::ranges::iterator_t<Base>>(),
                std::declval<std::ranges::iterator_t<Base>>()))) //
            requires std::indirectly_swappable<std::ranges::iterator_t<Base>>
        {
            for (std::size_t i = 0; i < N; ++i) {
                std::ranges::iter_swap(lhs.current_[i], rhs.current_[i]);
            }
        }

        constexpr auto& __current() const
        {
            return current_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr iterator(std::ranges::iterator_t<Base> first,
                           std::ranges::sentinel_t<Base> last)
        {
            current_[0] = first;
            for (std::size_t i = 1; i < N; ++i) {
                current_[i] = std::ranges::next(current_[i - 1], 1, last);
            }
        }

        constexpr iterator(__as_sentinel_t,
                           std::ranges::iterator_t<Base> first,
                           std::ranges::iterator_t<Base> last)
        {
            if constexpr (std::ranges::bidirectional_range<Base>) {
                current_[N - 1] = last;
                for (std::size_t i = 1; i < N; ++i) {
                    current_[N - i - 1]
                        = std::ranges::prev(current_[N - i], 1, first);
                }
            } else {
                for (std::size_t i = 0; i < N; ++i) {
                    current_[i] = last;
                }
            }
        }

        std::array<std::ranges::iterator_t<Base>, N> current_ {};
    };

    template <bool Const>
    class sentinel {
        friend class adjacent_view;

        using Base = __detail::__maybe_const<Const, View>;

    public:
        sentinel() = default;

        constexpr sentinel(sentinel<!Const> other) requires(
            Const&& std::convertible_to<std::ranges::sentinel_t<View>,
                                        std::ranges::sentinel_t<Base>>)
            : end_(std::move(other.end_))
        {
        }

        template <bool OtherConst>
            requires std::sentinel_for<
                std::ranges::sentinel_t<Base>,
                std::ranges::iterator_t<
                    __detail::__maybe_const<OtherConst, View>>>
        friend constexpr bool operator==(const iterator<OtherConst>& lhs,
                                         const sentinel& rhs)
        {
            return lhs.current_.back() == rhs.end_;
        }

        template <bool OtherConst>
            requires std::sized_sentinel_for<
                std::ranges::sentinel_t<Base>,
                std::ranges::iterator_t<
                    __detail::__maybe_const<OtherConst, View>>>
        friend constexpr std::ranges::range_difference_t<
            __detail::__maybe_const<OtherConst, View>>
        operator-(const iterator<OtherConst>& lhs, const sentinel& rhs)
        {
            return lhs.current_.back() - rhs.end_;
        }

        template <bool OtherConst>
            requires std::sized_sentinel_for<
                std::ranges::sentinel_t<Base>,
                std::ranges::iterator_t<
                    __detail::__maybe_const<OtherConst, View>>>
        friend constexpr std::ranges::range_difference_t<
            __detail::__maybe_const<OtherConst, View>>
        operator-(const sentinel& lhs, const iterator<OtherConst>& rhs)
        {
            return lhs.end_ - rhs.current_.back();
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit sentinel(std::ranges::sentinel_t<Base> end)
            : end_(end)
        {
        }

        std::ranges::sentinel_t<Base> end_ {};
    };

    adjacent_view() requires std::default_initializable<View>
    = default;

    constexpr explicit adjacent_view(View base)
        : base_(std::move(base))
    {
    }

    constexpr auto begin() requires(!__detail::__simple_view<View>)
    {
        return iterator<false>(std::ranges::begin(base_),
                               std::ranges::end(base_));
    }

    constexpr auto begin() const requires std::ranges::range<const View>
    {
        return iterator<true>(std::ranges::begin(base_),
                              std::ranges::end(base_));
    }

    constexpr auto end() requires(!__detail::__simple_view<View>)
    {
        if constexpr (std::ranges::common_range<View>) {
            return iterator<false>(__as_sentinel_t {},
                                   std::ranges::begin(base_),
                                   std::ranges::end(base_));
        } else {
            return sentinel<false>(std::ranges::end(base_));
        }
    }

    constexpr auto end() requires std::ranges::range<const View>
    {
        if constexpr (std::ranges::common_range<const View>) {
            return iterator<true>(__as_sentinel_t {}, std::ranges::begin(base_),
                                  std::ranges::end(base_));
        } else {
            return sentinel<true>(std::ranges::end(base_));
        }
    }

    constexpr auto size() requires std::ranges::sized_range<View>
    {
        using ST = decltype(std::ranges::size(base_));
        using CT = std::common_type_t<ST, size_t>;
        auto sz = static_cast<CT>(std::ranges::size(base_));
        sz -= std::min<CT>(sz, N - 1);
        return static_cast<ST>(sz);
    }

    constexpr auto size() const requires std::ranges::sized_range<const View>
    {
        using ST = decltype(std::ranges::size(base_));
        using CT = std::common_type_t<ST, size_t>;
        auto sz = static_cast<CT>(std::ranges::size(base_));
        sz -= std::min<CT>(sz, N - 1);
        return static_cast<ST>(sz);
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    View base_ {};
};

namespace views {

    template <std::size_t N>
    class __adjacent_fn : public range_adaptor_closure<__adjacent_fn<N>> {
    public:
        constexpr auto operator()() const noexcept
        {
            return std::views::empty<std::tuple<>>;
        }

        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(adjacent_view<std::views::all_t<Range&&>, N>(
                std::forward<Range>(range))))
                -> decltype(adjacent_view<std::views::all_t<Range&&>, N>(
                    std::forward<Range>(range)))
        {
            return adjacent_view<std::views::all_t<Range&&>, N>(
                std::forward<Range>(range));
        }
    };

    template <std::size_t N>
    inline constexpr __adjacent_fn<N> adjacent {};

    inline constexpr auto pairwise = adjacent<2>;
}
}

namespace iris {
namespace views = ranges::views;
}

namespace std::ranges {
template <class View, size_t N>
inline constexpr bool enable_borrowed_range<
    iris::ranges::adjacent_view<View, N>> = enable_borrowed_range<View>;
}
