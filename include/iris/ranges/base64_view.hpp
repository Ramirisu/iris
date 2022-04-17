#pragma once

#include <iris/config.hpp>

#include <iris/base64.hpp>
#include <iris/expected.hpp>
#include <iris/ranges/base.hpp>

namespace iris::ranges {

template <std::ranges::input_range View, typename Binary, typename Text>
    requires std::ranges::view<View>
class to_base64_view
    : public std::ranges::view_interface<to_base64_view<View, Binary, Text>> {
public:
    class iterator {
        using encoder = base64<Binary, Text>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<View>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category = std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<View>>::iterator_category,
                std::forward_iterator_tag>,
            std::forward_iterator_tag,
            std::input_iterator_tag>;
        using value_type = expected<Text, base64_error>;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<View>>
        = default;

        constexpr iterator(View& base, std::ranges::iterator_t<View> curr)
            : base_(std::addressof(base))
            , curr_(std::move(curr))
        {
            next();
            setup_result();
        }

        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;

        iterator(iterator&&) = default;
        iterator& operator=(iterator&&) = default;

        constexpr const std::ranges::iterator_t<View>& base() const& noexcept
        {
            return curr_;
        }

        constexpr std::ranges::iterator_t<View> base() && //
            noexcept(std::is_nothrow_move_constructible_v<
                     std::ranges::iterator_t<View>>)
        {
            return std::move(curr_);
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

        constexpr iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr bool operator==(std::default_sentinel_t) const
        {
            return !value_ && value_.error() == base64_error::eof;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_;
        }

    private:
        void next()
        {
            result_ = encoder::encode(curr_, std::ranges::end(*base_));
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

        View* base_ {};
        std::ranges::iterator_t<View> curr_ {};
        encoder::text_result_type result_ {};
        std::size_t offset_ {};
        value_type value_;
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

    constexpr iterator begin()
    {
        return { base_, std::ranges::begin(base_) };
    }

    constexpr std::default_sentinel_t end() noexcept
    {
        return {};
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
        constexpr __to_base64_fn() noexcept = default;

        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(to_base64_view {
                std::forward<Range>(range) })) requires requires
        {
            to_base64_view { std::forward<Range>(range) };
        }
        {
            return to_base64_view { std::forward<Range>(range) };
        }
    };

    inline constexpr __to_base64_fn to_base64 {};
}

template <std::ranges::input_range View, typename Binary, typename Text>
    requires std::ranges::view<View>
class from_base64_view
    : public std::ranges::view_interface<from_base64_view<View, Binary, Text>> {
public:
    class iterator {
        using encoder = base64<Binary, Text>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<View>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category = std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<View>>::iterator_category,
                std::forward_iterator_tag>,
            std::forward_iterator_tag,
            std::input_iterator_tag>;
        using value_type = expected<Binary, base64_error>;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<View>>
        = default;

        constexpr iterator(View& base, std::ranges::iterator_t<View> curr)
            : base_(std::addressof(base))
            , curr_(std::move(curr))
        {
            next();
            setup_result();
        }

        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;

        iterator(iterator&&) = default;
        iterator& operator=(iterator&&) = default;

        constexpr const std::ranges::iterator_t<View>& base() const& noexcept
        {
            return curr_;
        }

        constexpr std::ranges::iterator_t<View> base() && noexcept(
            std::is_nothrow_move_constructible_v<std::ranges::iterator_t<View>>)
        {
            return std::move(curr_);
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

        constexpr iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr bool operator==(std::default_sentinel_t) const
        {
            return !value_ && value_.error() == base64_error::eof;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_;
        }

    private:
        void next()
        {
            result_ = encoder::decode(curr_, std::ranges::end(*base_));
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

        View* base_ {};
        std::ranges::iterator_t<View> curr_ {};
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

    constexpr iterator begin()
    {
        return { base_, std::ranges::begin(base_) };
    }

    constexpr std::default_sentinel_t end() noexcept
    {
        return {};
    }

private:
    View base_;
};

template <typename Range>
from_base64_view(Range&&)
    -> from_base64_view<std::views::all_t<Range>,
                        std::uint8_t,
                        std::ranges::range_value_t<Range>>;

namespace views {
    class __from_base64_fn : public range_adaptor_closure<__from_base64_fn> {
    public:
        constexpr __from_base64_fn() noexcept = default;

        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(from_base64_view {
                std::forward<Range>(range) })) requires requires
        {
            from_base64_view { std::forward<Range>(range) };
        }
        {
            return from_base64_view { std::forward<Range>(range) };
        }
    };

    inline constexpr __from_base64_fn from_base64 {};
}

}

namespace iris {
namespace views = ranges::views;
}
