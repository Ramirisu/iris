#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {
namespace __unwrap_detail {
    template <typename T>
    concept __has_member_value = requires(T&& t)
    {
        // clang-format off
        { t.value() } -> __detail::__can_reference;
        // clang-format on
    };
}

template <std::ranges::input_range View>
    requires std::ranges::view<View> && __unwrap_detail::__has_member_value<
        std::ranges::range_value_t<View>>
class unwrap_view : public std::ranges::view_interface<unwrap_view<View>> {
public:
    template <bool Const>
    class iterator {
        friend class unwrap_view;

        using Base = __detail::__maybe_const<Const, View>;

    public:
        // clang-format off
        using iterator_category = 
            std::conditional_t<
                !std::is_lvalue_reference_v<std::ranges::range_reference_t<Base>>, 
                std::input_iterator_tag,
            std::conditional_t<
                std::derived_from<
                    typename std::iterator_traits<
                        std::ranges::iterator_t<Base>>::iterator_category,
                    std::random_access_iterator_tag>, 
                std::random_access_iterator_tag,
            std::conditional_t<
                std::derived_from<
                    typename std::iterator_traits<
                        std::ranges::iterator_t<Base>>::iterator_category, 
                    std::bidirectional_iterator_tag>, 
                std::bidirectional_iterator_tag,
            std::conditional_t<
                std::derived_from<
                    typename std::iterator_traits<
                        std::ranges::iterator_t<Base>>::iterator_category, 
                    std::forward_iterator_tag>, 
                std::forward_iterator_tag, 
                std::input_iterator_tag>>>>;
        // clang-format on
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
        using value_type =
            typename std::ranges::range_value_t<Base>::value_type;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr decltype(auto) operator*() const
        {
            return (*current_).value();
        }

        constexpr iterator& operator++()
        {
            ++current_;
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
            --current_;
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
            current_ += offset;
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            current_ -= offset;
            return *this;
        }

        constexpr auto operator[](difference_type offset) const //
            requires std::ranges::random_access_range<Base>
        {
            return *(*this + offset);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
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
                                            difference_type n) //
            requires std::ranges::random_access_range<Base>
        {
            auto r = i;
            r += n;
            return r;
        }

        friend constexpr iterator operator+(difference_type n,
                                            const iterator& i) //
            requires std::ranges::random_access_range<Base>
        {
            auto r = i;
            r += n;
            return r;
        }

        friend constexpr iterator operator-(const iterator& i,
                                            difference_type n) //
            requires std::ranges::random_access_range<Base>
        {
            auto r = i;
            r -= n;
            return r;
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   const iterator& rhs) //
            requires std::ranges::random_access_range<Base>
        {
            return lhs.current_ - rhs.current_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit iterator(std::ranges::iterator_t<Base> current)
            : current_(std::move(current))
        {
        }

        std::ranges::iterator_t<Base> current_ {};
    };

    template <bool Const>
    class sentinel {
        friend class unwrap_view;

        using Base = __detail::__maybe_const<Const, View>;

    public:
        template <bool OtherConst>
        friend constexpr bool operator==(const iterator<OtherConst>& lhs,
                                         const sentinel& rhs)
        {
            return lhs.current_ == rhs.end_;
        }

    private:
        constexpr explicit sentinel(std::ranges::sentinel_t<Base> end)
            : end_(std::move(end))
        {
        }

        std::ranges::sentinel_t<Base> end_ {};
    };

    unwrap_view() requires std::default_initializable<View>
    = default;

    constexpr explicit unwrap_view(View base)
        : base_(std::move(base))
    {
    }

    constexpr View base() const& requires std::copyable<View>
    {
        return base_;
    }

    constexpr View base() &&
    {
        return std::move(base_);
    }

    constexpr auto begin()
    {
        return iterator<false>(std::ranges::begin(base_));
    }

    constexpr auto begin() const requires std::ranges::range<const View>
    {
        return iterator<true>(std::ranges::begin(base_));
    }

    constexpr auto end()
    {
        if constexpr (std::ranges::common_range<View>) {
            return iterator<false>(std::ranges::end(base_));
        } else {
            return sentinel<false>(std::ranges::end(base_));
        }
    }

    constexpr auto end() const requires std::ranges::range<const View>
    {
        if constexpr (std::ranges::common_range<View>) {
            return iterator<true>(std::ranges::end(base_));
        } else {
            return sentinel<true>(std::ranges::end(base_));
        }
    }

    constexpr auto size() requires std::ranges::sized_range<View>
    {
        return std::ranges::size(base_);
    }

    constexpr auto size() const requires std::ranges::sized_range<const View>
    {
        return std::ranges::size(base_);
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    View base_ {};
};

template <typename Range>
unwrap_view(Range&&) -> unwrap_view<std::views::all_t<Range>>;

namespace views {
    class __unwrap_fn : public range_adaptor_closure<__unwrap_fn> {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(unwrap_view<std::views::all_t<Range&&>>(
                std::forward<Range>(range))))
                -> decltype(unwrap_view<std::views::all_t<Range&&>>(
                    std::forward<Range>(range)))
        {
            return unwrap_view<std::views::all_t<Range&&>>(
                std::forward<Range>(range));
        }
    };

    inline constexpr __unwrap_fn unwrap {};
}
}

namespace iris {
namespace views = ranges::views;
}
