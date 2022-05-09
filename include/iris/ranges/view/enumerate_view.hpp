#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {
namespace __enumerate_view_detail {
    template <typename Range>
    struct __index_type {
        using type
            = std::make_unsigned_t<std::ranges::range_difference_t<Range>>;
    };

    template <typename Range>
        requires std::ranges::sized_range<Range>
    struct __index_type<Range> {
        using type = std::ranges::range_size_t<Range>;
    };

    template <typename Range>
    using __index_type_t = typename __index_type<Range>::type;
}

template <typename Index, typename Value>
struct enumerate_result;

}

namespace std {

template <typename Index, typename Value>
struct tuple_size<iris::ranges::enumerate_result<Index, Value>>
    : integral_constant<size_t, 2> {
};

template <typename Index, typename Value>
struct tuple_element<0, iris::ranges::enumerate_result<Index, Value>> {
    using type = Index;
};

template <typename Index, typename Value>
struct tuple_element<1, iris::ranges::enumerate_result<Index, Value>> {
    using type = Value;
};

template <size_t I, typename Index, typename Value>
constexpr decltype(auto)
get(iris::ranges::enumerate_result<Index, Value>& r) noexcept
{
    return r.template get<I>();
}

template <size_t I, typename Index, typename Value>
constexpr decltype(auto)
get(const iris::ranges::enumerate_result<Index, Value>& r) noexcept
{
    return r.template get<I>();
}

template <size_t I, typename Index, typename Value>
constexpr decltype(auto)
get(iris::ranges::enumerate_result<Index, Value>&& r) noexcept
{
    return std::move(r).template get<I>();
}

template <size_t I, typename Index, typename Value>
constexpr decltype(auto)
get(const iris::ranges::enumerate_result<Index, Value>&& r) noexcept
{
    return std::move(r).template get<I>();
}

}

namespace iris::ranges {

template <typename Index, typename Value>
struct enumerate_result {
    Index index;
    Value value;

    template <std::size_t I>
    constexpr std::tuple_element_t<I, enumerate_result>& get() & noexcept
    {
        if constexpr (I == 0) {
            return index;
        } else {
            return value;
        }
    }

    template <std::size_t I>
    constexpr const std::tuple_element_t<I, enumerate_result>&
    get() const& noexcept
    {
        if constexpr (I == 0) {
            return index;
        } else {
            return value;
        }
    }

    template <std::size_t I>
    constexpr std::tuple_element_t<I, enumerate_result>&& get() && noexcept
    {
        if constexpr (I == 0) {
            return std::move(index);
        } else {
            return std::move(value);
        }
    }

    template <std::size_t I>
    constexpr const std::tuple_element_t<I, enumerate_result>&&
    get() const&& noexcept
    {
        if constexpr (I == 0) {
            return std::move(index);
        } else {
            return std::move(value);
        }
    }
};

template <std::ranges::input_range View>
    requires std::ranges::view<View>
class enumerate_view
    : public std::ranges::view_interface<enumerate_view<View>> {
public:
    template <bool Const>
    class iterator {
        friend class enumerate_view;

        using Base = __detail::__maybe_const<Const, View>;
        using index_type = __enumerate_view_detail::__index_type_t<Base>;

    public:
        using iterator_category = typename std::iterator_traits<
            std::ranges::iterator_t<Base>>::iterator_category;
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
                std::input_iterator_tag>>>; // STRENGTHEN:
        // clang-format on
        using value_type
            = std::tuple<index_type, std::ranges::range_value_t<Base>>;
        using reference
            = enumerate_result<index_type,
                               std::ranges::range_reference_t<Base>>;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!Const> other) requires(
            Const&& std::convertible_to<std::ranges::iterator_t<View>,
                                        std::ranges::iterator_t<Base>>)
            : current_(std::move(other.current_))
            , index_(static_cast<index_type>(other.index_))
        {
        }

        constexpr std::ranges::iterator_t<Base> base() const& //
            requires std::copyable<std::ranges::iterator_t<Base>>
        {
            return current_;
        }

        constexpr std::ranges::iterator_t<Base> base() &&
        {
            return std::move(current_);
        }

        constexpr decltype(auto) operator*() const
        {
            return reference { index_, *current_ };
        }

        constexpr iterator& operator++()
        {
            ++current_;
            ++index_;
            return *this;
        }

        constexpr decltype(auto) operator++(int)
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
            --index_;
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
            index_ += offset;
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            current_ -= offset;
            index_ -= offset;
            return *this;
        }

        constexpr decltype(auto) operator[](difference_type offset) const //
            requires std::ranges::random_access_range<Base>
        {
            return reference { static_cast<index_type>(
                                   static_cast<difference_type>(index_
                                                                + offset)),
                               *(current_ + offset) };
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs) requires
            std::equality_comparable<std::ranges::iterator_t<Base>>
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

        constexpr auto& __current() const
        {
            return current_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit iterator(std::ranges::iterator_t<Base> current,
                                    std::ranges::range_difference_t<Base> index)
            : current_(std::move(current))
            , index_(index)
        {
        }

        std::ranges::iterator_t<Base> current_ {};
        index_type index_ = 0;
    };

    template <bool Const>
    class sentinel {
        friend class enumerate_view;

        using Base = __detail::__maybe_const<Const, View>;

    public:
        sentinel() = default;

        constexpr sentinel(sentinel<!Const> other) //
            requires(Const&& std::convertible_to<std::ranges::sentinel_t<View>,
                                                 std::ranges::sentinel_t<Base>>)
            : end_(other.end_)
        {
        }

        constexpr std::ranges::sentinel_t<Base> base() const
        {
            return end_;
        }

        // note: not the same as what paper specified.
        template <bool OtherConst>
        friend constexpr bool operator==(const iterator<OtherConst>& lhs,
                                         const sentinel& rhs) //
            requires
            std::sentinel_for<std::ranges::sentinel_t<Base>,
                              std::ranges::iterator_t<
                                  __detail::__maybe_const<OtherConst, View>>>
        {
            return lhs.__current() == rhs.end_;
        }

        friend constexpr std::ranges::range_difference_t<Base>
        operator-(const iterator<Const>& lhs, const sentinel& rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>,
                                             std::ranges::iterator_t<Base>>
        {
            return lhs.current - rhs.end_;
        }

        friend constexpr std::ranges::range_difference_t<Base>
        operator-(const sentinel& lhs, const iterator<Const>& rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>,
                                             std::ranges::iterator_t<Base>>
        {
            return -(rhs - lhs);
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

    enumerate_view() requires std::default_initializable<View>
    = default;

    constexpr enumerate_view(View base)
        : base_(std::move(base))
    {
    }

    constexpr View base() const& requires std::copy_constructible<View>
    {
        return base_;
    }

    constexpr View base() &&
    {
        return move(base_);
    }

    constexpr auto begin() requires(!__detail::__simple_view<View>)
    {
        return iterator<false>(std::ranges::begin(base_), 0);
    }

    constexpr auto begin() const requires __detail::__simple_view<View>
    {
        return iterator<true>(std::ranges::begin(base_), 0);
    }

    constexpr auto end()
    {
        return sentinel<false>(std::ranges::end(base_));
    }

    constexpr auto end() //
        requires
        std::ranges::common_range<View> && std::ranges::sized_range<View>
    {
        return iterator<false>(
            std::ranges::end(base_),
            static_cast<std::ranges::range_difference_t<View>>(size()));
    }

    constexpr auto end() const //
        requires std::ranges::range<const View>
    {
        return sentinel<true> { std::ranges::end(base_) };
    }

    constexpr auto end() const //
        requires std::ranges::common_range<
            const View> && std::ranges::sized_range<const View>
    {
        return iterator<true> {
            std::ranges::end(base_),
            static_cast<std::ranges::range_difference_t<const View>>(size())
        };
    }

    constexpr auto size() //
        requires std::ranges::sized_range<View>
    {
        return std::ranges::size(base_);
    }

    constexpr auto size() const //
        requires std::ranges::sized_range<const View>
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
enumerate_view(Range&&) -> enumerate_view<std::views::all_t<Range>>;

namespace views {
    class __enumerate_fn : public range_adaptor_closure<__enumerate_fn> {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(enumerate_view(std::forward<Range>(range))))
                -> decltype(enumerate_view(std::forward<Range>(range)))
        {
            return enumerate_view(std::forward<Range>(range));
        }
    };

    inline constexpr __enumerate_fn enumerate {};
}
}

namespace iris {
namespace views = ranges::views;
}
