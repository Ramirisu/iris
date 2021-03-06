#pragma once

#include <iris/config.hpp>

#include <iris/bind.hpp>
#include <iris/ranges/__detail/non_propagating_cache.hpp>
#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>
#include <iris/utility.hpp>

#include <algorithm>

namespace iris::ranges {

template <std::ranges::view View>
    requires std::ranges::input_range<View>
class chunk_view : public std::ranges::view_interface<chunk_view<View>> {
public:
    class inner_iterator;

    class outer_iterator {
        friend class chunk_view;

    public:
        using iterator_concept = std::input_iterator_tag;
        using difference_type = std::ranges::range_difference_t<View>;

        outer_iterator() noexcept = default;

        outer_iterator(outer_iterator&&) = default;
        outer_iterator& operator=(outer_iterator&&) = default;

        class value_type : public std::ranges::view_interface<value_type> {
            friend class outer_iterator;

        public:
            constexpr inner_iterator begin() const noexcept
            {
                return inner_iterator { *parent_ };
            }

            constexpr std::default_sentinel_t end() const noexcept
            {
                return {};
            }

            constexpr auto size() const //
                requires std::sized_sentinel_for<std::ranges::sentinel_t<View>,
                                                 std::ranges::iterator_t<View>>
            {
                return std::ranges::min(parent_->remainder_,
                                        std::ranges::end(parent_->base_)
                                            - *parent_->current_);
            }

            friend constexpr difference_type
            operator-(std::default_sentinel_t, const outer_iterator& rhs)
                //
                requires std::sized_sentinel_for<std::ranges::sentinel_t<View>,
                                                 std::ranges::iterator_t<View>>
            {
                const auto dist = std::ranges::end(rhs.parent_->__base())
                    - *rhs.parent_->__current();

                if (dist < rhs.parent_->__remainder()) {
                    return dist == 0 ? 0 : 1;
                }

                return __detail::__div_ceil(dist - rhs.parent_->__remainder(),
                                            rhs.parent_->__n())
                    + 1;
            }

            friend constexpr difference_type
            operator-(const outer_iterator& lhs, std::default_sentinel_t rhs)
                //
                requires std::sized_sentinel_for<std::ranges::sentinel_t<View>,
                                                 std::ranges::iterator_t<View>>
            {
                return -(rhs - lhs);
            }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
            void __placeholder();
#endif

        private:
            constexpr value_type(chunk_view& parent)
                : parent_(std::addressof(parent))
            {
            }

            chunk_view* parent_;
        };

        constexpr value_type operator*() const
        {
            IRIS_ASSERT(*this != std::default_sentinel);
            return value_type(*parent_);
        }

        constexpr outer_iterator& operator++()
        {
            IRIS_ASSERT(*this != std::default_sentinel);
            std::ranges::advance(*parent_->current_, parent_->remainder_,
                                 std::ranges::end(parent_->base_));
            parent_->remainder_ = parent_->n_;

            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        friend constexpr bool operator==(const outer_iterator& lhs,
                                         std::default_sentinel_t)
        {
            return *lhs.parent_->__current()
                == std::ranges::end(lhs.parent_->__base())
                && lhs.parent_->__remainder() != 0;
        }

    private:
        constexpr explicit outer_iterator(chunk_view& parent)
            : parent_(std::addressof(parent))
        {
        }

        chunk_view* parent_ {};
    };

    class inner_iterator {
        friend class outer_iterator::value_type;

    public:
        using iterator_concept = std::input_iterator_tag;
        using difference_type = std::ranges::range_difference_t<View>;
        using value_type = std::ranges::range_value_t<View>;

        inner_iterator() noexcept = default;

        inner_iterator(inner_iterator&&) = default;
        inner_iterator& operator=(inner_iterator&&) = default;

        constexpr const std::ranges::iterator_t<View>& base() const&
        {
            return *parent_->current_;
        }

        constexpr std::ranges::range_reference_t<View> operator*() const
        {
            IRIS_ASSERT(*this != std::default_sentinel);
            return **parent_->current_;
        }

        constexpr inner_iterator& operator++()
        {
            IRIS_ASSERT(*this != std::default_sentinel);
            ++*parent_->current_;
            if (*parent_->current_ == std::ranges::end(parent_->base_)) {
                parent_->remainder_ = 0;
            } else {
                --parent_->remainder_;
            }

            return *this;
        }

        constexpr void operator++(int)
        {
            ++*this;
        }

        friend constexpr bool operator==(const inner_iterator& lhs,
                                         std::default_sentinel_t)
        {
            return lhs.get_remainder() == 0;
        }

        friend constexpr difference_type operator-(std::default_sentinel_t,
                                                   const inner_iterator& rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<View>,
                                             std::ranges::iterator_t<View>>
        {
            return std::ranges::min(rhs.parent_->remainder_,
                                    std::ranges::end(rhs.parent_->base_)
                                        - *rhs.parent_->current_);
        }

        friend constexpr difference_type
        operator-(const inner_iterator& lhs,
                  std::default_sentinel_t rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<View>,
                                             std::ranges::iterator_t<View>>
        {
            return -(rhs - lhs);
        }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
        void __placeholder();
#endif

    private:
        constexpr explicit inner_iterator(chunk_view& parent) noexcept
            : parent_(std::addressof(parent))
        {
        }

        constexpr auto get_remainder() const
        {
            return parent_->remainder_;
        }

        chunk_view* parent_ {};
    };

    chunk_view() requires std::default_initializable<View>
    = default;

    constexpr chunk_view(View base, std::ranges::range_difference_t<View> n)
        : base_(std::move(base))
        , n_(n)
    {
        IRIS_ASSERT(n > 0);
    }

    constexpr View base() const& requires std::copy_constructible<View>
    {
        return base_;
    }

    constexpr View base() &&
    {
        return std::move(base_);
    }

    constexpr outer_iterator begin()
    {
        current_.emplace(std::ranges::begin(base_));
        remainder_ = n_;
        return outer_iterator { *this };
    }

    constexpr std::default_sentinel_t end() noexcept
    {
        return {};
    }

    constexpr auto size() requires std::ranges::sized_range<View>
    {
        return to_unsigned(
            __detail::__div_ceil(std::ranges::distance(base_), n_));
    }

    constexpr auto size() const requires std::ranges::sized_range<const View>
    {
        return to_unsigned(
            __detail::__div_ceil(std::ranges::distance(base_), n_));
    }

    constexpr auto& __base()
    {
        return base_;
    }

    constexpr auto& __n()
    {
        return n_;
    }

    constexpr auto& __remainder()
    {
        return remainder_;
    }

    constexpr auto& __current()
    {
        return current_;
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    View base_ {};
    std::ranges::range_difference_t<View> n_ = 0;
    std::ranges::range_difference_t<View> remainder_ = 0;
    __detail::__non_propagating_cache<std::ranges::iterator_t<View>> current_;
};

template <std::ranges::view View>
    requires std::ranges::forward_range<View>
class chunk_view<View> : public std::ranges::view_interface<chunk_view<View>> {
public:
    template <bool Const>
    class iterator {
        friend class chunk_view;

        using Parent = __detail::__maybe_const<Const, chunk_view>;
        using Base = __detail::__maybe_const<Const, View>;

    public:
        using iterator_category = std::input_iterator_tag;
        using iterator_concept = std::conditional_t<
            std::ranges::random_access_range<Base>,
            std::random_access_iterator_tag,
            std::conditional_t<std::ranges::bidirectional_range<Base>,
                               std::bidirectional_iterator_tag,
                               std::forward_iterator_tag>>;
        using value_type = decltype(std::views::take(
            std::ranges::subrange(
                std::declval<std::ranges::iterator_t<Base>>(),
                std::declval<std::ranges::sentinel_t<Base>>()),
            std::declval<std::ranges::range_difference_t<Base>>()));
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!Const> other)
            // clang-format off
            requires(Const
                && std::convertible_to<
                    std::ranges::iterator_t<View>,
                    std::ranges::iterator_t<Base>> 
                && std::convertible_to<
                    std::ranges::sentinel_t<View>,
                    std::ranges::sentinel_t<Base>>)
            // clang-format on
            : current_(std::move(other.current_))
            , end_(std::move(other.end_))
            , n_(other.n_)
            , missing_(other.missing_)
        {
        }

        constexpr std::ranges::iterator_t<Base> base() const
        {
            return current_;
        }

        constexpr value_type operator*() const
        {
            IRIS_ASSERT(current_ != end_);
            return std::views::take(std::ranges::subrange(current_, end_), n_);
        }

        constexpr iterator& operator++()
        {
            IRIS_ASSERT(current_ != end_);
            missing_ = std::ranges::advance(current_, n_, end_);
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
            std::ranges::advance(current_, missing_ - n_);
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
            if (offset > 0) {
                IRIS_ASSERT(std::ranges::distance(current_, end_)
                            > n_ * (offset - 1));
                missing_ = std::ranges::advance(current_, n_ * offset, end_);
            } else if (offset < 0) {
                std::ranges::advance(current_, n_ * offset + missing_);
                missing_ = 0;
            }

            return *this;
        }

        constexpr iterator& operator-=(difference_type offset) //
            requires std::ranges::random_access_range<Base>
        {
            return *this += -offset;
        }

        constexpr value_type operator[](difference_type offset) const //
            requires std::ranges::random_access_range<Base>
        {
            return *(*this + offset);
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.current_ == rhs.current_;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         std::default_sentinel_t)
        {
            return lhs.current_ == lhs.end_;
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
                                          const iterator& rhs)
            // clang-format off
            requires std::ranges::random_access_range<Base>
                && std::three_way_comparable<std::ranges::iterator_t<Base>>
        // clang-format on
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
            return (lhs.current_ - rhs.current_ + lhs.missing_ - rhs.missing_)
                / lhs.n_;
        }

        friend constexpr difference_type operator-(std::default_sentinel_t,
                                                   const iterator& rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>,
                                             std::ranges::iterator_t<Base>>
        {
            return __detail::__div_ceil(rhs.end_, rhs.current_, rhs.n_);
        }

        friend constexpr difference_type
        operator-(const iterator& lhs,
                  std::default_sentinel_t rhs) //
            requires std::sized_sentinel_for<std::ranges::sentinel_t<Base>,
                                             std::ranges::iterator_t<Base>>
        {
            return -(rhs - lhs);
        }

        // clang-format off
    private:
        constexpr iterator(Parent& parent,
                           std::ranges::iterator_t<Base> current,
                           std::ranges::range_difference_t<Base> missing = 0)
            : current_(std::move(current))
            , end_(std::ranges::end(parent.base_))
            , n_(parent.n_)
            , missing_(missing)
        // clang-format on
        {
        }

        std::ranges::iterator_t<Base> current_;
        std::ranges::sentinel_t<Base> end_ {};
        std::ranges::range_difference_t<Base> n_ = 0;
        std::ranges::range_difference_t<Base> missing_ = 0;
    };

    chunk_view() requires std::default_initializable<View>
    = default;

    constexpr explicit chunk_view(View base,
                                  std::ranges::range_difference_t<View> n)
        : base_(std::move(base))
        , n_(n)
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

    constexpr auto begin() requires(!__detail::__simple_view<View>)
    {
        return iterator<false> { *this, std::ranges::begin(base_) };
    }

    constexpr auto begin() const requires std::ranges::forward_range<const View>
    {
        return iterator<true> { *this, std::ranges::begin(base_) };
    }

    constexpr auto end() requires(!__detail::__simple_view<View>)
    {
        // clang-format off
        if constexpr (std::ranges::common_range<View> 
            && std::ranges::sized_range<View>) {
            auto missing = (n_ - std::ranges::distance(base_) % n_) % n_;
            return iterator<false> { *this, std::ranges::end(base_), missing };
        } else if constexpr (std::ranges::common_range<View> 
            && !std::ranges::bidirectional_range<View>) {
            return iterator<false> { *this, std::ranges::end(base_) };
        } else {
            return std::default_sentinel;
        }
        // clang-format on
    }

    constexpr auto end() const requires std::ranges::forward_range<const View>
    {
        // clang-format off
        if constexpr (std::ranges::common_range<View> 
            && std::ranges::sized_range<View>) {
            auto missing = (n_ - std::ranges::distance(base_) % n_) % n_;
            return iterator<true> { *this, std::ranges::end(base_), missing };
        } else if constexpr (std::ranges::common_range<View> 
            && !std::ranges::bidirectional_range<View>) {
            return iterator<true> { *this, std::ranges::end(base_) };
        } else {
            return std::default_sentinel;
        }
        // clang-format on
    }

    constexpr auto size() requires std::ranges::sized_range<View>
    {
        return to_unsigned(
            __detail::__div_ceil(std::ranges::distance(base_), n_));
    }

    constexpr auto size() const requires std::ranges::sized_range<const View>
    {
        return to_unsigned(
            __detail::__div_ceil(std::ranges::distance(base_), n_));
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    View base_ {};
    std::ranges::range_difference_t<View> n_ = 0;
};

template <typename Range>
chunk_view(Range&&, std::ranges::range_difference_t<Range>)
    -> chunk_view<std::views::all_t<Range>>;

namespace views {
    struct __chunk_fn {
        template <std::ranges::viewable_range Range>
        constexpr auto
        operator()(Range&& range,
                   const std::ranges::range_difference_t<Range> n) const
            noexcept(noexcept(chunk_view(std::forward<Range>(range), n)))
                -> decltype(chunk_view(std::forward<Range>(range), n))
        {
            return chunk_view(std::forward<Range>(range), n);
        }

        template <typename N>
        constexpr auto operator()(N&& n) const noexcept(
            std::is_nothrow_constructible_v<std::decay_t<N>, N>) requires
            std::constructible_from<std::decay_t<N>, N>
        {
            return range_adaptor_closure(bind_back(*this, std::forward<N>(n)));
        }
    };

    inline constexpr __chunk_fn chunk {};
}
}

namespace iris {
namespace views = ranges::views;
}

namespace std::ranges {
template <class View>
inline constexpr bool enable_borrowed_range<
    iris::ranges::chunk_view<View>> = forward_range<View>&&
    enable_borrowed_range<View>;
}
