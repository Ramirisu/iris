#pragma once

#include <iris/config.hpp>

#include <iris/base64.hpp>
#include <iris/expected.hpp>

#include <ranges>

namespace iris::ranges {

template <std::ranges::input_range Range, typename Binary, typename Text>
    requires std::ranges::view<Range>
class base64_encode_view : public std::ranges::view_interface<
                               base64_encode_view<Range, Binary, Text>> {
public:
    class iterator {
        using encoder = base64<Binary, Text>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<Range>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category = std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Range>>::iterator_category,
                std::forward_iterator_tag>,
            std::forward_iterator_tag,
            std::input_iterator_tag>;
        using value_type = expected<Text, base64_error>;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<Range>>
        = default;

        constexpr iterator(Range& parent, std::ranges::iterator_t<Range> curr)
            : parent_(&parent)
            , curr_(std::move(curr))
        {
            next();
            setup_result();
        }

        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;

        iterator(iterator&&) = default;
        iterator& operator=(iterator&&) = default;

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

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_;
        }

    private:
        void next()
        {
            result_ = encoder::encode(curr_, std::ranges::end(*parent_));
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

        Range* parent_ {};
        std::ranges::iterator_t<Range> curr_ {};
        encoder::text_result_type result_ {};
        std::size_t offset_ {};
        value_type value_;
    };

    class sentinel {
    public:
        sentinel() = default;

        constexpr sentinel(std::ranges::sentinel_t<Range> last)
            : last_(std::move(last))
        {
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const sentinel& rhs)
        {
            return !*lhs && (*lhs).error() == base64_error::eof;
        }

    private:
        std::ranges::sentinel_t<Range> last_;
    };

    // clang-format off
    base64_encode_view() requires std::default_initializable<Range> = default;
    // clang-format on

    constexpr explicit base64_encode_view(Range range) noexcept(
        std::is_nothrow_move_constructible_v<Range>)
        : range_(std::move(range))
    {
    }

    // clang-format off
    [[nodiscard]] constexpr Range base() const& 
        noexcept(std::is_nothrow_copy_constructible_v<Range>) 
        requires std::copy_constructible<Range>
    // clang-format on
    {
        return range_;
    }

    // clang-format off
    [[nodiscard]] constexpr Range base() && 
        noexcept(std::is_nothrow_move_constructible_v<Range>) 
        requires std::move_constructible<Range>
    // clang-format on
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

    // clang-format off
    [[nodiscard]] constexpr auto size()  
        noexcept(noexcept(std::ranges::size(range_))) 
        requires std::ranges::sized_range<Range>
    // clang-format on
    {
        return (std::ranges::size(range_) + 2) / 3 * 4;
    }

    // clang-format off
private:
    // clang-format on
    Range range_;
};

template <typename Range>
base64_encode_view(Range&&)
    -> base64_encode_view<std::views::all_t<Range>,
                          std::ranges::range_value_t<Range>,
                          std::uint8_t>;

namespace views {
#if defined(__GNUC__) && __GNUC__ == 10
    inline constexpr std::ranges::views::__adaptor::_RangeAdaptorClosure
        base64_encode
        = []<std::ranges::viewable_range Range>(Range&& range) {
              return base64_encode_view<std::views::all_t<Range>,
                                        std::ranges::range_value_t<Range>,
                                        std::uint8_t> { std::forward<Range>(
                  range) };
          };
#else
    struct __base64_encode_view_fn
#if defined(_MSC_VER)
        : std::ranges::_Pipe::_Base<__base64_encode_view_fn>
#elif defined(__GNUC__) && __GNUC__ >= 11
        : std::ranges::views::__adaptor::_RangeAdaptorClosure
#endif
    {
        template <std::ranges::viewable_range Range>
        [[nodiscard]] constexpr auto operator()(Range&& range) const
        {
            return base64_encode_view<std::views::all_t<Range>,
                                      std::ranges::range_value_t<Range>,
                                      std::uint8_t> { std::forward<Range>(
                range) };
        }
    };

    inline constexpr __base64_encode_view_fn base64_encode {};
#endif
}

template <std::ranges::input_range Range, typename Binary, typename Text>
    requires std::ranges::view<Range>
class base64_decode_view : public std::ranges::view_interface<
                               base64_decode_view<Range, Binary, Text>> {
public:
    class iterator {
        using encoder = base64<Binary, Text>;

    public:
        using iterator_concept
            = std::conditional_t<std::ranges::forward_range<Range>,
                                 std::forward_iterator_tag,
                                 std::input_iterator_tag>;
        using iterator_category = std::conditional_t<
            std::derived_from<
                typename std::iterator_traits<
                    std::ranges::iterator_t<Range>>::iterator_category,
                std::forward_iterator_tag>,
            std::forward_iterator_tag,
            std::input_iterator_tag>;
        using value_type = expected<Binary, base64_error>;
        using difference_type = std::ptrdiff_t;

        iterator() requires
            std::default_initializable<std::ranges::iterator_t<Range>>
        = default;

        constexpr iterator(Range& parent, std::ranges::iterator_t<Range> curr)
            : parent_(&parent)
            , curr_(std::move(curr))
        {
            next();
            setup_result();
        }

        iterator(const iterator&) = default;
        iterator& operator=(const iterator&) = default;

        iterator(iterator&&) = default;
        iterator& operator=(iterator&&) = default;

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

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.curr_ == rhs.curr_;
        }

    private:
        void next()
        {
            result_ = encoder::decode(curr_, std::ranges::end(*parent_));
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

        Range* parent_ {};
        std::ranges::iterator_t<Range> curr_ {};
        encoder::binary_result_type result_ {};
        std::size_t offset_ {};
        value_type value_;
    };

    class sentinel {
    public:
        sentinel() = default;

        constexpr sentinel(std::ranges::sentinel_t<Range> last)
            : last_(std::move(last))
        {
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const sentinel& rhs)
        {
            return !*lhs && (*lhs).error() == base64_error::eof;
        }

    private:
        std::ranges::sentinel_t<Range> last_;
    };

    // clang-format off
    base64_decode_view() requires std::default_initializable<Range> = default;
    // clang-format on

    constexpr explicit base64_decode_view(Range range) noexcept(
        std::is_nothrow_move_constructible_v<Range>)
        : range_(std::move(range))
    {
    }

    // clang-format off
    [[nodiscard]] constexpr Range base() const& 
        noexcept(std::is_nothrow_copy_constructible_v<Range>) 
        requires std::copy_constructible<Range>
    // clang-format on
    {
        return range_;
    }

    // clang-format off
    [[nodiscard]] constexpr Range base() && 
        noexcept(std::is_nothrow_move_constructible_v<Range>) 
        requires std::move_constructible<Range>
    // clang-format on
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
    Range range_;
};

template <typename Range>
base64_decode_view(Range&&)
    -> base64_decode_view<std::views::all_t<Range>,
                          std::uint8_t,
                          std::ranges::range_value_t<Range>>;

namespace views {
#if defined(__GNUC__) && __GNUC__ == 10
    inline constexpr std::ranges::views::__adaptor::_RangeAdaptorClosure
        base64_decode
        = []<std::ranges::viewable_range Range>(Range&& range) {
              return base64_decode_view<std::views::all_t<Range>,
                                        std::ranges::range_value_t<Range>,
                                        std::uint8_t> { std::forward<Range>(
                  range) };
          };
#else
    struct __base64_decode_view_fn
#if defined(_MSC_VER)
        : std::ranges::_Pipe::_Base<__base64_decode_view_fn>
#elif defined(__GNUC__) && __GNUC__ >= 11
        : std::ranges::views::__adaptor::_RangeAdaptorClosure
#endif
    {
        template <std::ranges::viewable_range Range>
        [[nodiscard]] constexpr auto operator()(Range&& range) const
        {
            return base64_decode_view<std::views::all_t<Range>, std::uint8_t,
                                      std::ranges::range_value_t<Range>> {
                std::forward<Range>(range)
            };
        }
    };

    inline constexpr __base64_decode_view_fn base64_decode {};
#endif
}

}

namespace iris {
namespace views = ranges::views;
}
