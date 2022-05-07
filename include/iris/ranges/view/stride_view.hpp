#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {

template <std::ranges::input_range View>
    requires std::ranges::view<View>
class stride_view : public std::ranges::view_interface<stride_view<View>> {
public:
    template <typename Base>
    class iterator_base {
    };
    template <typename Base>
        requires std::ranges::forward_range<Base>
    class iterator_base<Base> {
    public:
        // clang-format off
        using iterator_category = std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Base>>::iterator_category,
                std::random_access_iterator_tag>,
            std::random_access_iterator_tag,
            typename std::iterator_traits<
                std::ranges::iterator_t<Base>>::iterator_category>;
        // clang-format on
    };

    template <bool Const>
    class iterator
        : public iterator_base<__detail::__maybe_const<Const, View>> {
        friend class stride_view;

        using Parent = __detail::__maybe_const<Const, stride_view>;
        using Base = __detail::__maybe_const<Const, View>;

    public:
        // clang-format off
        using iterator_concept = 
            std::conditional_t<
                std::ranges::random_access_range<Base>,
                std::random_access_iterator_tag,
            std::conditional_t<
                std::ranges::bidirectional_range<Base>,
                std::bidirectional_iterator_tag,
            std::conditional_t<
                    std::ranges::forward_range<Base>,
                    std::forward_iterator_tag,
                    std::input_iterator_tag>>>;
        // clang-format on
        using value_type = std::ranges::range_value_t<Base>;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!Const> other) requires(
            Const&& std::convertible_to<std::ranges::iterator_t<View>,
                                        std::ranges::iterator_t<Base>>&&
                std::convertible_to<std::ranges::sentinel_t<View>,
                                    std::ranges::sentinel_t<Base>>)
            : current_(std::move(other.current_))
            , end_(std::move(other.end_))
            , stride_(std::move(other.stride_))
            , missing_(std::move(other.missing_))
        {
        }

        constexpr const std::ranges::iterator_t<Base>& base() const& noexcept
        {
            return current_;
        }

        constexpr std::ranges::iterator_t<Base> base() &&
        {
            return std::move(current_);
        }

        constexpr decltype(auto) operator*() const
        {
            return *current_;
        }

        constexpr iterator& operator++()
        {
            IRIS_ASSERT(current_ != end_);
            missing_ = std::ranges::advance(current_, stride_, end_);
            return *this;
        }

        constexpr auto operator++(int)
        {
            if constexpr (std::ranges::forward_range<Base>) {
                auto tmp = *this;
                ++*this;
                return tmp;
            } else {
                ++*this;
            }
        }

        constexpr iterator& operator--() //
            requires std::ranges::bidirectional_range<Base>
        {
            std::ranges::advance(current_, missing_ - stride_);
            missing_ = 0;
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
            IRIS_ASSERT(offset <= 0
                        || std::ranges::distance(current_, end_)
                            > stride_ * (offset - 1));
            if (offset > 0) {
                missing_
                    = std::ranges::advance(current_, stride_ * offset, end_);
            } else if (offset < 0) {
                std::ranges::advance(current_, stride_ * offset + missing_);
                missing_ = 0;
            }
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            return *this += -offset;
        }

        constexpr decltype(auto) operator[](difference_type offset) const //
            requires std::ranges::random_access_range<Base>
        {
            return *(*this + offset);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         std::default_sentinel_t)
        {
            return lhs.current_ == lhs.end_;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs) //
            requires std::equality_comparable<std::ranges::iterator_t<Base>>
        {
            return lhs.current_ == rhs.current_;
        }

        friend constexpr bool operator<(const iterator& lhs,
                                        const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return lhs.current_ < rhs.current_;
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
            return lhs.current_ <=> rhs.current_;
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
            const auto n = lhs.current_ - rhs.current_;
            if constexpr (std::ranges::forward_range<Base>) {
                return (n + lhs.missing_ - rhs.missing_) / lhs.stride_;
            } else {
                if (n < 0) {
                    return -__detail::__div_ceil(-n, lhs.stride_);
                } else {
                    return __detail::__div_ceil(n, lhs.stride_);
                }
            }
        }

        friend constexpr difference_type operator-(std::default_sentinel_t,
                                                   const iterator& rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>,
                                             std::ranges::iterator_t<Base>>
        {
            return __detail::__div_ceil(rhs.end_ - rhs.current_, rhs.stride_);
        }

        friend constexpr difference_type
        operator-(const iterator& lhs,
                  std::default_sentinel_t rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>,
                                             std::ranges::iterator_t<Base>>
        {
            return -(rhs - lhs);
        }

        friend constexpr std::ranges::range_rvalue_reference_t<Base>
        iter_move(const iterator& i) noexcept(
            noexcept(std::ranges::iter_move(i.current_)))
        {
            return std::ranges::iter_move(i.current_);
        }

        friend constexpr void
        iter_swap(const iterator& lhs, const iterator& rhs) noexcept(
            noexcept(std::ranges::iter_swap(lhs.current_, rhs.current_))) //
            requires std::indirectly_swappable<std::ranges::iterator_t<Base>>
        {
            return std::ranges::iter_swap(lhs.current_, rhs.current_);
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr iterator(Parent* parent,
                           std::ranges::iterator_t<Base> current,
                           std::ranges::range_difference_t<Base> missing = 0)
            : current_(std::move(current))
            , end_(std::ranges::end(parent->base_))
            , stride_(parent->stride_)
            , missing_(missing)
        {
        }

        std::ranges::iterator_t<Base> current_ {};
        std::ranges::sentinel_t<Base> end_ {};
        std::ranges::range_difference_t<Base> stride_ = 0;
        std::ranges::range_difference_t<Base> missing_ = 0;
    };

    stride_view() requires std::default_initializable<View>
    = default;

    constexpr explicit stride_view(View base,
                                   std::ranges::range_difference_t<View> stride)
        : base_(std::move(base))
        , stride_(stride)
    {
    }
    constexpr View base() const& requires std::copy_constructible<View>
    {
        return base_;
    }

    constexpr View base() &&
    {
        return std::move(base_);
    }

    constexpr std::ranges::range_difference_t<View> stride() const noexcept
    {
        return stride_;
    }

    constexpr auto begin() requires(!__detail::__simple_view<View>)
    {
        return iterator<false>(this, std::ranges::begin(base_));
    }

    constexpr auto begin() const requires std::ranges::range<const View>
    {
        return iterator<true>(this, std::ranges::begin(base_));
    }

    constexpr auto end() requires(!__detail::__simple_view<View>)
    {
        // clang-format off
        if constexpr (std::ranges::common_range<View> 
            && std::ranges::sized_range<View> 
            && std::ranges::forward_range<View>) {
            // clang-format on
            auto missing
                = (stride_ - std::ranges::distance(base_) % stride_) % stride_;
            return iterator<false>(this, std::ranges::end(base_), missing);
            // clang-format off
        } else if constexpr (std::ranges::common_range<View> 
            && !std::ranges::bidirectional_range<View>) {
            // clang-format on
            return iterator<false>(this, std::ranges::end(base_));
        } else {
            return std::default_sentinel;
        }
    }

    constexpr auto end() const requires std::ranges::range<const View>
    {
        // clang-format off
        if constexpr (std::ranges::common_range<const View> 
            && std::ranges::sized_range<const View> 
            && std::ranges::forward_range<const View>) {
            // clang-format on
            auto missing
                = (stride_ - std::ranges::distance(base_) % stride_) % stride_;
            return iterator<true>(this, std::ranges::end(base_), missing);
            // clang-format off
        } else if constexpr (std::ranges::common_range<const View> 
            && !std::ranges::bidirectional_range<const View>) {
            // clang-format on
            return iterator<true>(this, std::ranges::end(base_));
        } else {
            return std::default_sentinel;
        }
    }

    constexpr auto size() //
        requires std::ranges::sized_range<View>
    {
        auto value
            = __detail::__div_ceil(std::ranges::distance(base_), stride_);
        return static_cast<std::make_unsigned_t<decltype(value)>>(value);
    }

    constexpr auto size() const //
        requires std::ranges::sized_range<const View>
    {
        auto value
            = __detail::__div_ceil(std::ranges::distance(base_), stride_);
        return static_cast<std::make_unsigned_t<decltype(value)>>(value);
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    View base_ {};
    std::ranges::range_difference_t<View> stride_ = 1;
};

template <class Range>
stride_view(Range&&, std::ranges::range_difference_t<Range>)
    -> stride_view<std::views::all_t<Range>>;

namespace views {
    class __stride_fn {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto
        operator()(Range&& range,
                   std::ranges::range_difference_t<Range> n) const
            noexcept(noexcept(stride_view(std::forward<Range>(range),
                                          n))) requires requires
        {
            stride_view(std::forward<Range>(range), n);
        }
        {
            return stride_view(std::forward<Range>(range), n);
        }

        template <typename N>
        constexpr auto operator()(N&& n) const noexcept(
            std::is_nothrow_constructible_v<std::decay_t<N>, N>) requires
            std::constructible_from<std::decay_t<N>, N>
        {
            return range_adaptor_closure<__stride_fn, std::decay_t<N>>(
                std::forward<N>(n));
        }
    };

    inline constexpr __stride_fn stride {};
}
}

namespace iris {
namespace views = ranges::views;
}
