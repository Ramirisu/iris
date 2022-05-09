#pragma once

#include <iris/config.hpp>

#include <iris/ranges/__detail/copyable_box.hpp>
#include <iris/ranges/__detail/utility.hpp>
#include <iris/utility.hpp>

namespace iris::ranges {

template <std::copy_constructible Value,
          std::semiregular Bound = std::unreachable_sentinel_t>
    // clang-format off
    requires(std::is_object_v<Value>
        && std::same_as<Value, std::remove_cv_t<Value>> 
        && (std::integral<Bound> || std::same_as<Bound, std::unreachable_sentinel_t>))
// clang-format on
class repeat_view
    : public std::ranges::view_interface<repeat_view<Value, Bound>> {
public:
    class iterator {
        friend class repeat_view;

        // using index_type = std::conditional_t<
        //     std::same_as<Bound, std::unreachable_sentinel_t>,
        //     std::ptrdiff_t,
        //     Bound>;
        using index_type = std::ptrdiff_t;

        constexpr explicit iterator(const Value* value,
                                    index_type current = index_type())
            : value_(value)
            , current_(current)
        {
        }

    public:
        using iterator_concept = std::random_access_iterator_tag;
        using iterator_category = std::random_access_iterator_tag;
        using value_type = Value;
        using reference = const Value&;
        using difference_type = std::ptrdiff_t;

        iterator() = default;

        constexpr const Value& operator*() const noexcept
        {
            return *value_;
        }

        constexpr iterator& operator++()
        {
            ++current_;
            return *this;
        }

        constexpr iterator operator++(int)
        {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        constexpr iterator& operator--()
        {
            --current_;
            return *this;
        }

        constexpr iterator operator--(int)
        {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        constexpr iterator& operator+=(difference_type offset)
        {
            current_ += offset;
            return *this;
        }

        constexpr iterator& operator-=(difference_type offset)
        {
            current_ -= offset;
            return *this;
        }

        constexpr const Value& operator[](difference_type offset) const noexcept
        {
            IRIS_UNUSED(offset);
            return *value_;
        }

        friend constexpr bool operator==(const iterator& lhs,
                                         const iterator& rhs)
        {
            return lhs.current_ == rhs.current_;
        }

        friend constexpr auto operator<=>(const iterator& lhs,
                                          const iterator& rhs)
        {
            return lhs.current_ <=> rhs.current_;
        }

        friend constexpr iterator operator+(iterator i, difference_type offset)
        {
            return iterator { i.value_, i.current_ + offset };
        }

        friend constexpr iterator operator+(difference_type offset, iterator i)
        {
            return iterator { i.value_, i.current_ + offset };
        }

        friend constexpr iterator operator-(iterator i, difference_type offset)
        {
            return iterator { i.value_, i.current_ - offset };
        }

        friend constexpr difference_type operator-(const iterator& lhs,
                                                   const iterator& rhs)
        {
            return lhs.current_ - rhs.current_;
        }

    private:
        const Value* value_ {};
        index_type current_ {};
    };

    repeat_view() requires std::default_initializable<Value>
    = default;

    constexpr explicit repeat_view(const Value& value, Bound bound = Bound())
        : value_(std::in_place, value)
        , bound_(bound)
    {
    }

    constexpr explicit repeat_view(Value&& value, Bound bound = Bound())
        : value_(std::in_place, std::move(value))
        , bound_(bound)
    {
    }

    template <class... WArgs, class... BoundArgs>
        requires std::constructible_from<Value, WArgs...> && std::
            constructible_from<Bound, BoundArgs...>
    constexpr explicit repeat_view(std::piecewise_construct_t,
                                   std::tuple<WArgs...> value_args,
                                   std::tuple<BoundArgs...> bound_args
                                   = std::tuple<> {})
        : repeat_view(std::move(value_args),
                      std::move(bound_args),
                      std::index_sequence_for<WArgs...> {},
                      std::index_sequence_for<BoundArgs...> {})
    {
    }

    constexpr iterator begin() const
    {
        return iterator { std::addressof(*value_) };
    }

    constexpr iterator end() const
        requires(!std::same_as<Bound, std::unreachable_sentinel_t>)
    {
        return iterator { std::addressof(*value_), bound_ };
    }

    constexpr std::unreachable_sentinel_t end() const
    {
        return {};
    }

    constexpr auto size() const
        requires(!std::same_as<Bound, std::unreachable_sentinel_t>)
    {
        return to_unsigned(bound_);
    }

#if IRIS_FIX_CLANG_FORMAT_PLACEHOLDER
    void __placeholder();
#endif

private:
    template <class... WArgs,
              class... BoundArgs,
              std::size_t... WIs,
              std::size_t... BIs>
        requires std::constructible_from<Value, WArgs...> && std::
            constructible_from<Bound, BoundArgs...>
    constexpr explicit repeat_view(std::tuple<WArgs...> value_args,
                                   std::tuple<BoundArgs...> bound_args,
                                   std::index_sequence<WIs...>,
                                   std::index_sequence<BIs...>)
        : value_(std::in_place, std::get<WIs>(std::move(value_args))...)
        , bound_(std::in_place, std::get<BIs>(std::move(bound_args))...)
    {
    }

    __detail::__copyable_box<Value> value_ {};
    Bound bound_ {};
};

template <typename Value, typename Bound>
repeat_view(Value, Bound) -> repeat_view<Value, Bound>;

namespace views {
    class __repeat_fn {
    public:
        template <typename Value>
        constexpr auto operator()(Value&& value) const
            noexcept(noexcept(repeat_view(std::forward<Value>(value))))
                -> decltype(repeat_view(std::forward<Value>(value)))
        {
            return repeat_view(std::forward<Value>(value));
        }

        template <typename Value, typename Bound>
        constexpr auto operator()(Value&& value, Bound&& bound) const
            noexcept(noexcept(repeat_view(std::forward<Value>(value),
                                          std::forward<Bound>(bound))))
                -> decltype(repeat_view(std::forward<Value>(value),
                                        std::forward<Bound>(bound)))
        {
            return repeat_view(std::forward<Value>(value),
                               std::forward<Bound>(bound));
        }
    };

    inline constexpr __repeat_fn repeat {};
}

}

namespace iris {
namespace views = ranges::views;
}
