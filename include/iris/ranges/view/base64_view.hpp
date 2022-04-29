#pragma once

#include <iris/config.hpp>

#include <iris/base64.hpp>
#include <iris/expected.hpp>
#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {

template <std::ranges::input_range View, typename Binary, typename Text>
    requires std::ranges::view<View>
class to_base64_view
    : public std::ranges::view_interface<to_base64_view<View, Binary, Text>> {
public:
    template <bool IsConst>
    class iterator {
        friend class to_base64_view;

        using Parent = __detail::__maybe_const<IsConst, to_base64_view>;
        using Base = __detail::__maybe_const<IsConst, View>;
        using encoder = base64<Binary, Text>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<Base>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category = std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Base>>::iterator_category,
                std::forward_iterator_tag>,
            std::forward_iterator_tag,
            std::input_iterator_tag>;
        using value_type = Text;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!IsConst> other) requires(
            IsConst&& std::convertible_to<std::ranges::iterator_t<View>,
                                          std::ranges::iterator_t<Base>>)
            : parent_(other.parent_)
            , curr_(std::move(other.curr_))
            , result_(std::move(other.result_))
            , offset_(other.offset_)
        {
        }

        constexpr const value_type& operator*() const noexcept
        {
            IRIS_ASSERT(result_);
            return result_.value()[offset_];
        }

        constexpr iterator& operator++()
        {
            if (result_) {
                ++offset_;
                if (offset_ == result_.value().size()) {
                    next();
                }
            } else {
                next();
            }

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

        constexpr bool operator==(std::default_sentinel_t) const
        {
            return !result_ && result_.error() == base64_error::eof;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_ && lhs.result_ == rhs.result_
                && lhs.offset_ == rhs.offset_;
        }

    private:
        constexpr explicit iterator(Parent& parent,
                                    std::ranges::iterator_t<Base> curr)
            : parent_(std::addressof(parent))
            , curr_(std::move(curr))
        {
            next();
        }

        void next()
        {
            result_ = encoder::encode(curr_, std::ranges::end(parent_->base_));
            offset_ = 0;
        }

        Parent* parent_ {};
        std::ranges::iterator_t<Base> curr_ {};
        encoder::text_result_type result_ {};
        std::size_t offset_ {};
    };

    // clang-format off
    to_base64_view()
        requires std::default_initializable<View> = default;
    // clang-format on

    constexpr explicit to_base64_view(View view) //
        noexcept(std::is_nothrow_move_constructible_v<View>)
        : base_(std::move(view))
    {
    }

    constexpr View base() const& //
        noexcept(std::is_nothrow_copy_constructible_v<View>) //
        requires std::copy_constructible<View>
    {
        return base_;
    }

    constexpr View base() && //
        noexcept(std::is_nothrow_move_constructible_v<View>) //
        requires std::move_constructible<View>
    {
        return std::move(base_);
    }

    constexpr auto begin()
    {
        return iterator<false>(*this, std::ranges::begin(base_));
    }

    constexpr auto begin() const //
        requires std::ranges::range<const View>
    {
        return iterator<true>(*this, std::ranges::begin(base_));
    }

    constexpr auto end()
    {
        if constexpr (std::ranges::common_range<View>) {
            return iterator<false>(*this, std::ranges::end(base_));
        } else {
            return std::default_sentinel;
        }
    }

    constexpr auto end() const //
        requires std::ranges::range<const View>
    {
        if constexpr (std::ranges::common_range<const View>) {
            return iterator<true>(*this, std::ranges::end(base_));
        } else {
            return std::default_sentinel;
        }
    }

    constexpr auto size() //
        noexcept(noexcept(std::ranges::size(base_))) //
        requires std::ranges::sized_range<View>
    {
        return (std::ranges::size(base_) + 2) / 3 * 4;
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    View base_;
};

template <typename Range>
to_base64_view(Range&&) -> to_base64_view<std::views::all_t<Range>,
                                          std::ranges::range_value_t<Range>,
                                          std::uint8_t>;

namespace views {
    class __to_base64_fn : public range_adaptor_closure<__to_base64_fn> {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const noexcept(noexcept(
            to_base64_view(std::forward<Range>(range)))) requires requires
        {
            to_base64_view(std::forward<Range>(range));
        }
        {
            return to_base64_view(std::forward<Range>(range));
        }
    };

    inline constexpr __to_base64_fn to_base64 {};
}

template <std::ranges::input_range View, typename Binary, typename Text>
    requires std::ranges::view<View>
class from_base64_view
    : public std::ranges::view_interface<from_base64_view<View, Binary, Text>> {
public:
    template <bool IsConst>
    class iterator {
        friend class from_base64_view;

        using Parent = __detail::__maybe_const<IsConst, from_base64_view>;
        using Base = __detail::__maybe_const<IsConst, View>;
        using encoder = base64<Binary, Text>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<Base>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category = std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Base>>::iterator_category,
                std::forward_iterator_tag>,
            std::forward_iterator_tag,
            std::input_iterator_tag>;
        using value_type = expected<Binary, base64_error>;
        using difference_type = std::ranges::range_difference_t<Base>;

        iterator() = default;

        constexpr iterator(iterator<!IsConst> other) requires(
            IsConst&& std::convertible_to<std::ranges::iterator_t<View>,
                                          std::ranges::iterator_t<Base>>)
            : parent_(other.parent_)
            , curr_(std::move(other.curr_))
            , result_(std::move(other.result_))
            , offset_(other.offset_)
            , value_(std::move(other.value_))
        {
        }

        constexpr const value_type& operator*() const noexcept
        {
            return value_;
        }

        constexpr iterator& operator++()
        {
            if (result_) {
                ++offset_;
                if (offset_ == result_.value().size()) {
                    next();
                }
            } else {
                next();
            }

            setup_result();
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

        constexpr bool operator==(std::default_sentinel_t) const
        {
            return !value_ && value_.error() == base64_error::eof;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_ && lhs.result_ == rhs.result_
                && lhs.offset_ == rhs.offset_;
        }

    private:
        constexpr explicit iterator(Parent& parent,
                                    std::ranges::iterator_t<Base> curr)
            : parent_(std::addressof(parent))
            , curr_(std::move(curr))
        {
            next();
            setup_result();
        }

        void next()
        {
            result_ = encoder::decode(curr_, std::ranges::end(parent_->base_));
            offset_ = 0;
        }

        void setup_result()
        {
            if (result_) {
                value_ = result_.value()[offset_];
            } else {
                value_ = unexpected(result_.error());
            }
        }

        Parent* parent_ {};
        std::ranges::iterator_t<Base> curr_ {};
        encoder::binary_result_type result_ {};
        std::size_t offset_ {};
        value_type value_;
    };

    from_base64_view() requires std::default_initializable<View>
    = default;

    constexpr explicit from_base64_view(View base) noexcept(
        std::is_nothrow_move_constructible_v<View>)
        : base_(std::move(base))
    {
    }

    constexpr View base() const& //
        noexcept(std::is_nothrow_copy_constructible_v<View>) //
        requires std::copy_constructible<View>
    {
        return base_;
    }

    constexpr View base() && //
        noexcept(std::is_nothrow_move_constructible_v<View>) //
        requires std::move_constructible<View>
    {
        return std::move(base_);
    }

    constexpr auto begin()
    {
        return iterator<false>(*this, std::ranges::begin(base_));
    }

    constexpr auto begin() const //
        requires std::ranges::range<const View>
    {
        return iterator<true>(*this, std::ranges::begin(base_));
    }

    constexpr auto end()
    {
        if constexpr (std::ranges::common_range<View>) {
            return iterator<false>(*this, std::ranges::end(base_));
        } else {
            return std::default_sentinel;
        }
    }

    constexpr auto end() const //
        requires std::ranges::range<const View>
    {
        if constexpr (std::ranges::common_range<const View>) {
            return iterator<true>(*this, std::ranges::end(base_));
        } else {
            return std::default_sentinel;
        }
    }

private : View base_;
};

template <typename Range>
from_base64_view(Range&&)
    -> from_base64_view<std::views::all_t<Range>,
                        std::uint8_t,
                        std::ranges::range_value_t<Range>>;

namespace views {
    class __from_base64_fn : public range_adaptor_closure<__from_base64_fn> {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const noexcept(noexcept(
            from_base64_view(std::forward<Range>(range)))) requires requires
        {
            from_base64_view(std::forward<Range>(range));
        }
        {
            return from_base64_view(std::forward<Range>(range));
        }
    };

    inline constexpr __from_base64_fn from_base64 {};
}

}

namespace iris {
namespace views = ranges::views;
}
