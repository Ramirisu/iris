#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/utility.hpp>
#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {

template <std::ranges::input_range View>
    requires std::ranges::view<View>
class as_rvalue_view
    : public std::ranges::view_interface<as_rvalue_view<View>> {
public:
    as_rvalue_view() requires std::default_initializable<View>
    = default;

    constexpr explicit as_rvalue_view(View base)
        : base_(std::move(base))
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
        return std::move_iterator(std::ranges::begin(base_));
    }

    constexpr auto begin() const requires std::ranges::range<const View>
    {
        return std::move_iterator(std::ranges::begin(base_));
    }

    constexpr auto end() requires(!__detail::__simple_view<View>)
    {
        if constexpr (std::ranges::common_range<View>) {
            return std::move_iterator(std::ranges::end(base_));
        } else {
            return std::move_sentinel(std::ranges::end(base_));
        }
    }

    constexpr auto end() const requires std::ranges::range<const View>
    {
        if constexpr (std::ranges::common_range<View>) {
            return std::move_iterator(std::ranges::end(base_));
        } else {
            return std::move_sentinel(std::ranges::end(base_));
        }
    }

    constexpr auto size() requires std::ranges::sized_range<View>
    {
        return std::ranges::size(base_);
    }

    constexpr auto size() const requires std::ranges::sized_range<View>
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
as_rvalue_view(Range&&) -> as_rvalue_view<std::views::all_t<Range>>;

namespace views {
    class __as_rvalue_fn : public range_adaptor_closure<__as_rvalue_fn> {
    public:
        template <std::ranges::viewable_range Range>
            requires std::same_as<std::ranges::range_rvalue_reference_t<Range>,
                                  std::ranges::range_reference_t<Range>>
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(std::views::all(std::forward<Range>(range))))
                -> decltype(std::views::all(std::forward<Range>(range)))
        {
            return std::views::all(std::forward<Range>(range));
        }

        template <std::ranges::viewable_range Range>
            requires(!std::same_as<std::ranges::range_rvalue_reference_t<Range>,
                                   std::ranges::range_reference_t<Range>>)
        constexpr auto operator()(Range&& range) const
            noexcept(noexcept(as_rvalue_view(std::forward<Range>(range))))
                -> decltype(as_rvalue_view(std::forward<Range>(range)))
        {

            return as_rvalue_view(std::forward<Range>(range));
        }
    };

    inline constexpr __as_rvalue_fn as_rvalue {};
}
}

namespace iris {
namespace views = ranges::views;
}

namespace std::ranges {
template <class T>
inline constexpr bool enable_borrowed_range<
    iris::ranges::as_rvalue_view<T>> = enable_borrowed_range<T>;
}
