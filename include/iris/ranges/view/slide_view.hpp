#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {
namespace __slide_view_detail {
    template <typename View>
    concept __slide_caches_nothing = std::ranges::random_access_range<
        View> && std::ranges::sized_range<View>;

    template <class View>
    concept __slide_caches_last
        = !__slide_caches_nothing<
              View> && std::ranges::bidirectional_range<View> && std::ranges::common_range<View>;

    template <class View>
    concept __slide_caches_first
        = !__slide_caches_nothing<View> && !__slide_caches_last<View>;
}

template <std::ranges::forward_range View>
    requires std::ranges::view<View>
class slide_view : public std::ranges::view_interface<slide_view<View>> {
public:
    template <typename Base>
    class iterator_base {
    };

    template <typename Base>
        requires __slide_view_detail::__slide_caches_first<Base>
    class iterator_base<Base> {
    public:
        iterator_base() = default;

    protected:
        constexpr iterator_base(std::ranges::iterator_t<Base> last_element)
            : last_element_(last_element)
        {
        }

        std::ranges::iterator_t<Base> last_element_ {};
    };

    template <bool IsConst>
    class iterator
        : public iterator_base<__detail::__maybe_const<IsConst, View>> {
        friend class slide_view;

        using Base = __detail::__maybe_const<IsConst, View>;

        constexpr iterator(std::ranges::iterator_t<Base> current,
                           std::ranges::range_difference_t<Base> n) //
            requires(!__slide_view_detail::__slide_caches_first<Base>)
            : current_(current)
            , n_(n)
        {
        }

        constexpr iterator(std::ranges::iterator_t<Base> current,
                           std::ranges::iterator_t<Base> last_element,
                           std::ranges::range_difference_t<Base> n) //
            requires(__slide_view_detail::__slide_caches_first<Base>)
            : iterator_base<__detail::__maybe_const<IsConst, View>>(
                last_element)
            , current_(current)
            , n_(n)
        {
        }

    public:
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::conditional_t<
            std::ranges::random_access_range<Base>,
            std::random_access_iterator_tag,
            std::conditional_t<std::ranges::bidirectional_range<Base>,
                               std::bidirectional_iterator_tag,
                               std::forward_iterator_tag>>;
        using value_type = decltype(std::views::counted(
            std::declval<std::ranges::iterator_t<Base>>(),
            std::declval<std::ranges::range_difference_t<Base>>()));
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!IsConst> other) //
            requires(IsConst //
                         && std::convertible_to<std::ranges::iterator_t<View>,
                                                std::ranges::iterator_t<Base>>)
            : current_(std::move(other.current_))
            , n_(other.n_)
        {
        }

        constexpr auto operator*() const
        {
            return std::views::counted(current_, n_);
        }

        constexpr iterator& operator++()
        {
            current_ = std::ranges::next(current_);
            if constexpr (__slide_view_detail::__slide_caches_first<Base>) {
                this->last_element_ = std::ranges::next(this->last_element_);
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
            current_ = std::ranges::prev(current_);
            if constexpr (__slide_view_detail::__slide_caches_first<Base>) {
                this->last_element_ = std::ranges::prev(this->last_element_);
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
            current_ = std::ranges::next(current_, offset);
            if constexpr (__slide_view_detail::__slide_caches_first<Base>) {
                this->last_element_
                    = std::ranges::next(this->last_element_, offset);
            }

            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            current_ = std::ranges::prev(current_, offset);
            if constexpr (__slide_view_detail::__slide_caches_first<Base>) {
                this->last_element_
                    = std::ranges::prev(this->last_element_, offset);
            }

            return *this;
        }

        constexpr auto operator[](difference_type offset) const //
            requires std::ranges::random_access_range<Base>
        {
            return std::views::counted(current_ + offset, n_);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            if constexpr (__slide_view_detail::__slide_caches_first<Base>) {
                return lhs.last_element_ == rhs.last_element_;
            } else {
                return lhs.current_ == rhs.current_;
            }
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
            if constexpr (__slide_view_detail::__slide_caches_first<Base>) {
                return lhs.last_element_ - rhs.last_element_;
            } else {
                return lhs.current_ - rhs.current_;
            }
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        std::ranges::iterator_t<Base> current_ {};
        std::ranges::range_difference_t<Base> n_ = 0;
    };

    template <bool IsConst>
    class sentinel {
    public:
        sentinel() = default;

        friend constexpr bool operator==(const iterator<false>& lhs,
                                         const sentinel& rhs)
        {
            return lhs.last_element_ == rhs.end_;
        }

        friend constexpr std::ranges::range_difference_t<View>
        operator-(const iterator<false>& lhs, const sentinel& rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<View>,
                                             std::ranges::iterator_t<View>>
        {
            return lhs.last_element_ - rhs.end_;
        }

        friend constexpr std::ranges::range_difference_t<View>
        operator-(const sentinel& lhs, const iterator<false>& rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<View>,
                                             std::ranges::iterator_t<View>>
        {
            return lhs.end_ - rhs.last_element_;
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif
    private:
        constexpr explicit sentinel(std::ranges::sentinel_t<View> end)
            : end_(end)
        {
        }

        std::ranges::sentinel_t<View> end_ {};
    };

    // clang-format off
    slide_view()
        requires std::default_initializable<View> = default;
    // clang-format on

    constexpr explicit slide_view(View base,
                                  std::ranges::range_difference_t<View> n)
        : base_(std::move(base))
        , n_(n)
    {
    }

    constexpr auto begin() //
        requires(
            !(__detail::__simple_view<
                  View> && __slide_view_detail::__slide_caches_nothing<View>))
    {
        if constexpr (__slide_view_detail::__slide_caches_first<View>) {
            return iterator<false>(std::ranges::begin(base_),
                                   std::ranges::next(std::ranges::begin(base_),
                                                     n_ - 1,
                                                     std::ranges::end(base_)),
                                   n_);
        } else {
            return iterator<false>(std::ranges::begin(base_), n_);
        }
    }

    constexpr auto begin() const //
        requires __slide_view_detail::__slide_caches_nothing<const View>
    {
        return iterator<true>(std::ranges::begin(base_), n_);
    }

    constexpr auto end() //
        requires(
            !(__detail::__simple_view<
                  View> && __slide_view_detail::__slide_caches_nothing<View>))
    {
        if constexpr (__slide_view_detail::__slide_caches_nothing<View>) {
            return iterator<false>(
                std::ranges::begin(base_)
                    + std::ranges::range_difference_t<View>(size()),
                n_);
        } else if constexpr (__slide_view_detail::__slide_caches_last<View>) {
            return iterator<false>(std::ranges::prev(std ::ranges::end(base_),
                                                     n_ - 1,
                                                     std::ranges::begin(base_)),
                                   n_);
        } else if constexpr (std::ranges::common_range<View>) {
            return iterator<false>(std::ranges::end(base_),
                                   std::ranges::end(base_), n_);
        } else {
            return sentinel(std::ranges::end(base_));
        }
    }

    constexpr auto end() const //
        requires __slide_view_detail::__slide_caches_nothing<const View>
    {
        return begin() + std::ranges::range_difference_t<const View>(size());
    }

    constexpr auto size() requires std::ranges::sized_range<View>
    {
        auto sz = std::ranges::distance(base_) - n_ + 1;
        if (sz < 0) {
            sz = 0;
        }
        return static_cast<std::make_unsigned_t<decltype(sz)>>(sz);
    }

    constexpr auto size() const requires std::ranges::sized_range<const View>
    {
        auto sz = std::ranges::distance(base_) - n_ + 1;
        if (sz < 0) {
            sz = 0;
        }
        return static_cast<std::make_unsigned_t<decltype(sz)>>(sz);
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif
private:
    View base_ {};
    std::ranges::range_difference_t<View> n_ = 0;
};

template <typename Range>
slide_view(Range&&, std::ranges::range_difference_t<Range>)
    -> slide_view<std::views::all_t<Range>>;

namespace views {
    class __slide_fn {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto
        operator()(Range&& range,
                   const std::ranges::range_difference_t<Range> n) const
            noexcept(noexcept(slide_view(std::forward<Range>(range),
                                         n))) requires requires
        {
            slide_view(std::forward<Range>(range), n);
        }
        {
            return slide_view(std::forward<Range>(range), n);
        }

        template <typename N>
        constexpr auto operator()(N&& n) const noexcept(
            std::is_nothrow_constructible_v<std::decay_t<N>, N>) requires
            std::constructible_from<std::decay_t<N>, N>
        {
            return range_adaptor_closure<__slide_fn, std::decay_t<N>>(
                std::forward<N>(n));
        }
    };

    inline constexpr __slide_fn slide {};
}

}

namespace iris {
namespace views = ranges::views;
}
