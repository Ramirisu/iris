#pragma once

#include <iris/config.hpp>

#include <iris/expected.hpp>
#include <iris/utf.hpp>

#include <ranges>

namespace iris {

template <std::ranges::input_range Range, typename Unicode, typename UTF>
    // clang-format off
    requires std::ranges::view<Range>
// clang-format on
class from_utf_view
    : public std::ranges::view_interface<from_utf_view<Range, Unicode, UTF>> {
public:
    class iterator {
        using __base_iterator_category = typename std::iterator_traits<
            std::ranges::iterator_t<Range>>::iterator_category;
        using utf_type = utf<Unicode, UTF>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<Range>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category
            = std::conditional_t<std::derived_from<__base_iterator_category,
                                                   std::forward_iterator_tag>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using value_type = typename utf_type::unicode_result_type;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<Range>>
        = default;

        constexpr iterator(Range& parent, std::ranges::iterator_t<Range> curr)
            : parent_(&parent)
            , curr_(curr)
        {
            next();
        }

        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;

        iterator(iterator&&) = default;
        iterator& operator=(iterator&&) = default;

        constexpr const value_type& operator*() const
        {
            return result_;
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

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_;
        }

    private:
        void next()
        {
            result_
                = utf<Unicode, UTF>::decode(curr_, std::ranges::end(*parent_));
        }

        Range* parent_ {};
        std::ranges::iterator_t<Range> curr_ {};
        value_type result_;
    };

    class sentinel {
    public:
        sentinel() = default;

        constexpr sentinel(std::ranges::sentinel_t<Range> last)
            : last_(last)
        {
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const sentinel& rhs)
        {
            return !*lhs && (*lhs).error() == utf_error::eof;
        }

    private:
        std::ranges::sentinel_t<Range> last_;
    };

    // clang-format off
    from_utf_view() requires std::default_initializable<Range> = default;
    // clang-format on

    constexpr explicit from_utf_view(Range range) noexcept(
        std::is_nothrow_move_constructible_v<Range>)
        : range_(std::move(range))
    {
    }

    [[nodiscard]] constexpr Range
    base() const& noexcept(std::is_nothrow_copy_constructible_v<Range>) requires
        std::copy_constructible<Range>
    {
        return range_;
    }

    [[nodiscard]] constexpr Range
    base() && noexcept(std::is_nothrow_move_constructible_v<Range>) requires
        std::move_constructible<Range>
    {
        return std::move(range_);
    }

    [[nodiscard]] constexpr iterator begin()
    {
        return { range_, std::ranges::begin(range_) };
    }

    [[nodiscard]] constexpr sentinel end()
    {
        return { std::ranges::end(range_) };
    }

private:
    Range range_ {};
};

template <typename Range>
from_utf_view(Range&&) -> from_utf_view<std::ranges::views::all_t<Range>,
                                        std::uint32_t,
                                        std::ranges::range_value_t<Range>>;

namespace views {
#if defined(__GNUC__) && __GNUC__ == 10
    inline constexpr std::ranges::views::__adaptor::_RangeAdaptorClosure
        from_utf
        = []<std::ranges::viewable_range Range>(Range&& range) {
              return from_utf_view { std::forward<Range>(range) };
          };
#else
    struct __from_utf_view_fn
#if defined(_MSC_VER)
        : std::ranges::_Pipe::_Base<__from_utf_view_fn>
#elif defined(__GNUC__) && __GNUC__ >= 11
        : std::ranges::views::__adaptor::_RangeAdaptorClosure
#endif
    {
        template <std::ranges::viewable_range Range>
        [[nodiscard]] constexpr auto operator()(Range&& range) const
            // clang-format off
            noexcept(noexcept(from_utf_view { std::forward<Range>(range) }))
            requires requires { from_utf_view { std::forward<Range>(range) }; }
        // clang-format on
        {
            return from_utf_view { std::forward<Range>(range) };
        }
    };

    inline constexpr __from_utf_view_fn from_utf {};
#endif
}

template <std::ranges::input_range Range, typename Unicode, typename UTF>
    // clang-format off
    requires std::ranges::view<Range>
// clang-format on
class to_utf_view
    : public std::ranges::view_interface<to_utf_view<Range, Unicode, UTF>> {
public:
    class iterator {
        using __base_iterator_category = typename std::iterator_traits<
            std::ranges::iterator_t<Range>>::iterator_category;
        using utf_type = utf<Unicode, UTF>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<Range>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category
            = std::conditional_t<std::derived_from<__base_iterator_category,
                                                   std::forward_iterator_tag>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using value_type = expected<UTF, utf_error>;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<Range>>
        = default;

        constexpr iterator(Range& parent, std::ranges::iterator_t<Range> curr)
            : parent_(&parent)
            , curr_(curr)
        {
            next();
            setup_result();
        }

        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;

        iterator(iterator&&) = default;
        iterator& operator=(iterator&&) = default;

        constexpr const value_type& operator*() const
        {
            return result_;
        }

        constexpr iterator& operator++()
        {
            if (utf_result_) {
                ++offset_;
                if (offset_ == utf_result_.value().size()) {
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

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_;
        }

    private:
        void next()
        {
            utf_result_
                = utf<Unicode, UTF>::encode(curr_, std::ranges::end(*parent_));
            offset_ = 0;
        }

        void setup_result()
        {
            if (utf_result_) {
                result_ = utf_result_.value()[offset_];
            } else {
                result_ = unexpected(utf_result_.error());
            }
        }

        Range* parent_ {};
        std::ranges::iterator_t<Range> curr_ {};
        typename utf_type::utf_result_type utf_result_ {};
        std::size_t offset_ {};
        value_type result_;
    };

    class sentinel {
    public:
        sentinel() = default;

        constexpr sentinel(std::ranges::sentinel_t<Range> last)
            : last_(last)
        {
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const sentinel& rhs)
        {
            return !*lhs && (*lhs).error() == utf_error::eof;
        }

    private:
        std::ranges::sentinel_t<Range> last_;
    };

    // clang-format off
    to_utf_view() requires std::default_initializable<Range> = default;
    // clang-format on

    constexpr explicit to_utf_view(Range range) noexcept(
        std::is_nothrow_move_constructible_v<Range>)
        : range_(std::move(range))
    {
    }

    [[nodiscard]] constexpr Range
    base() const& noexcept(std::is_nothrow_copy_constructible_v<Range>) requires
        std::copy_constructible<Range>
    {
        return range_;
    }

    [[nodiscard]] constexpr Range
    base() && noexcept(std::is_nothrow_move_constructible_v<Range>) requires
        std::move_constructible<Range>
    {
        return std::move(range_);
    }

    [[nodiscard]] constexpr iterator begin()
    {
        return { range_, std::ranges::begin(range_) };
    }

    [[nodiscard]] constexpr sentinel end()
    {
        return { std::ranges::end(range_) };
    }

private:
    Range range_ {};
};

namespace views {
#if defined(__GNUC__) && __GNUC__ == 10
    inline constexpr std::ranges::views::__adaptor::_RangeAdaptorClosure
        to_utf8_char
        = []<std::ranges::viewable_range Range>(Range&& range) {
              return to_utf_view<std::views::all_t<Range>, std::uint32_t,
                                 char> { std::forward<Range>(range) };
          };
#else
    struct __to_utf8_char_view_fn
#if defined(_MSC_VER)
        : std::ranges::_Pipe::_Base<__to_utf8_char_view_fn>
#elif defined(__GNUC__) && __GNUC__ >= 11
        : std::ranges::views::__adaptor::_RangeAdaptorClosure
#endif
    {
        template <std::ranges::viewable_range Range>
        [[nodiscard]] constexpr auto operator()(Range&& range) const
        {
            return to_utf_view<std::views::all_t<Range>, std::uint32_t, char> {
                std::forward<Range>(range)
            };
        }
    };

    inline constexpr __to_utf8_char_view_fn to_utf8_char {};
#endif

#if defined(__GNUC__) && __GNUC__ == 10
    inline constexpr std::ranges::views::__adaptor::_RangeAdaptorClosure to_utf8
        = []<std::ranges::viewable_range Range>(Range&& range) {
              return to_utf_view<std::views::all_t<Range>, std::uint32_t,
                                 char8_t> { std::forward<Range>(range) };
          };
#else
    struct __to_utf8_view_fn
#if defined(_MSC_VER)
        : std::ranges::_Pipe::_Base<__to_utf8_view_fn>
#elif defined(__GNUC__) && __GNUC__ >= 11
        : std::ranges::views::__adaptor::_RangeAdaptorClosure
#endif
    {
        template <std::ranges::viewable_range Range>
        [[nodiscard]] constexpr auto operator()(Range&& range) const
        {
            return to_utf_view<std::views::all_t<Range>, std::uint32_t,
                               char8_t> { std::forward<Range>(range) };
        }
    };

    inline constexpr __to_utf8_view_fn to_utf8 {};
#endif

#if defined(__GNUC__) && __GNUC__ == 10
    inline constexpr std::ranges::views::__adaptor::_RangeAdaptorClosure
        to_utf16
        = []<std::ranges::viewable_range Range>(Range&& range) {
              return to_utf_view<std::views::all_t<Range>, std::uint32_t,
                                 char16_t> { std::forward<Range>(range) };
          };
#else
    struct __to_utf16_view_fn
#if defined(_MSC_VER)
        : std::ranges::_Pipe::_Base<__to_utf16_view_fn>
#elif defined(__GNUC__) && __GNUC__ >= 11
        : std::ranges::views::__adaptor::_RangeAdaptorClosure
#endif
    {
        template <std::ranges::viewable_range Range>
        [[nodiscard]] constexpr auto operator()(Range&& range) const
        {
            return to_utf_view<std::views::all_t<Range>, std::uint32_t,
                               char16_t> { std::forward<Range>(range) };
        }
    };

    inline constexpr __to_utf16_view_fn to_utf16 {};
#endif

#if defined(__GNUC__) && __GNUC__ == 10
    inline constexpr std::ranges::views::__adaptor::_RangeAdaptorClosure
        to_utf32
        = []<std::ranges::viewable_range Range>(Range&& range) {
              return to_utf_view<std::views::all_t<Range>, std::uint32_t,
                                 char32_t> { std::forward<Range>(range) };
          };
#else
    struct __to_utf32_view_fn
#if defined(_MSC_VER)
        : std::ranges::_Pipe::_Base<__to_utf32_view_fn>
#elif defined(__GNUC__) && __GNUC__ >= 11
        : std::ranges::views::__adaptor::_RangeAdaptorClosure
#endif
    {
        template <std::ranges::viewable_range Range>
        [[nodiscard]] constexpr auto operator()(Range&& range) const
        {
            return to_utf_view<std::views::all_t<Range>, std::uint32_t,
                               char32_t> { std::forward<Range>(range) };
        }
    };

    inline constexpr __to_utf32_view_fn to_utf32 {};
#endif
}
}
