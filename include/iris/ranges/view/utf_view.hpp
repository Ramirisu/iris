#pragma once

#include <iris/config.hpp>

#include <iris/expected.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>
#include <iris/utf.hpp>

namespace iris::ranges {

template <std::ranges::input_range View, typename Unicode, typename UTF>
    requires std::ranges::view<View>
class to_utf_view
    : public std::ranges::view_interface<to_utf_view<View, Unicode, UTF>> {
public:
    class iterator {
        using encoder = utf<Unicode, UTF>;

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
        using value_type = expected<UTF, utf_error>;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<View>>
        = default;

        constexpr iterator(View& base, std::ranges::iterator_t<View> curr)
            : base_(&base)
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
            return !value_ && value_.error() == utf_error::eof;
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
        encoder::utf_result_type result_ {};
        std::size_t offset_ {};
        value_type value_;
    };

    to_utf_view() requires std::default_initializable<View>
    = default;

    constexpr explicit to_utf_view(View base) noexcept(
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

    constexpr auto size() //
        noexcept(noexcept(std::ranges::size(base_))) //
        requires(sizeof(UTF) == 4 && std::ranges::sized_range<View>)
    {
        return std::ranges::size(base_);
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    View base_ {};
};

namespace views {

    template <typename UTF>
    class __to_utf_fn : public range_adaptor_closure<__to_utf_fn<UTF>> {
    public:
        constexpr __to_utf_fn() noexcept = default;

        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(to_utf_view<std::ranges::views::all_t<Range>,
                                          std::ranges::range_reference_t<Range>,
                                          UTF> {
                std::forward<Range>(range) })) requires requires
        {
            to_utf_view<std::ranges::views::all_t<Range>,
                        std::ranges::range_reference_t<Range>, UTF> {
                std::forward<Range>(range)
            };
        }
        {
            return to_utf_view<std::ranges::views::all_t<Range>,
                               std::ranges::range_reference_t<Range>, UTF> {
                std::forward<Range>(range)
            };
        }
    };

    template <typename UTF>
    inline constexpr __to_utf_fn<UTF> to_utf {};
}

template <std::ranges::input_range View, typename Unicode, typename UTF>
    requires std::ranges::view<View>
class from_utf_view
    : public std::ranges::view_interface<from_utf_view<View, Unicode, UTF>> {
public:
    class iterator {
        using encoder = utf<Unicode, UTF>;

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
        using value_type = typename encoder::unicode_result_type;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<View>>
        = default;

        constexpr iterator(View& base, std::ranges::iterator_t<View> curr)
            : base_(&base)
            , curr_(std::move(curr))
        {
            next();
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
            next();
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
            return !value_ && value_.error() == utf_error::eof;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_;
        }

    private:
        void next()
        {
            value_ = encoder::decode(curr_, std::ranges::end(*base_));
        }

        View* base_ {};
        std::ranges::iterator_t<View> curr_ {};
        value_type value_;
    };

    from_utf_view() requires std::default_initializable<View>
    = default;

    constexpr explicit from_utf_view(View base) //
        noexcept(std::is_nothrow_move_constructible_v<View>)
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

    constexpr auto size() //
        noexcept(noexcept(std::ranges::size(base_))) //
        requires(sizeof(UTF) == 4 && std::ranges::sized_range<View>)
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
from_utf_view(Range&&) -> from_utf_view<std::ranges::views::all_t<Range>,
                                        std::uint32_t,
                                        std::ranges::range_value_t<Range>>;

namespace views {
    class __from_utf_fn : public range_adaptor_closure<__from_utf_fn> {
    public:
        constexpr __from_utf_fn() noexcept = default;

        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(from_utf_view {
                std::forward<Range>(range) })) requires requires
        {
            from_utf_view { std::forward<Range>(range) };
        }
        {
            return from_utf_view { std::forward<Range>(range) };
        }
    };

    inline constexpr __from_utf_fn from_utf {};
}

}

namespace iris {
namespace views = ranges::views;
}