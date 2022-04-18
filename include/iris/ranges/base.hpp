#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {

// clang-format off
template <typename Range>
concept __simple_view = std::ranges::view<Range> 
    && std::ranges::range<Range> 
    && std::same_as<
        std::ranges::iterator_t<Range>, 
        std::ranges::iterator_t<const Range>> 
    && std::same_as<
        std::ranges::sentinel_t<Range>, 
        std::ranges::sentinel_t<const Range>>;
// clang-format on

template <typename T>
using __with_reference = T&;

template <typename T>
concept __can_reference = requires
{
    typename __with_reference<T>;
};

template <bool IsConst, typename T>
using __maybe_const = std::conditional_t<IsConst, const T, T>;

template <typename T>
#if defined(_MSC_VER)
using __copyable_box = std::ranges::_Copyable_box<T>;
#elif defined(__GNUC__)
class __copyable_box : public std::ranges::__detail::__box<T> {
public:
    // gcc's impl doesn't have this...
    template <typename... Args>
    __copyable_box(std::in_place_t, Args&&... args)
        : std::ranges::__detail::__box<T>(std::forward<Args>(args)...)
    {
    }

    // gcc's impl doesn't have this...
    constexpr operator bool() const noexcept
    {
        return this->has_value();
    }
};
#else
#error "Unsupported compiler."
#endif

template <typename T>
    requires std::is_object_v<T>
class __non_propagating_cache {
public:
    constexpr __non_propagating_cache() noexcept {};

    constexpr ~__non_propagating_cache()
    {
        if (has_) {
            value_.~T();
        }
    }

    constexpr __non_propagating_cache(
        const __non_propagating_cache&) noexcept = default;

    constexpr __non_propagating_cache(__non_propagating_cache&& other)
    {
        if (other.has_) {
            other.value_.~T();
            other.has_ = false;
        }
    }

    constexpr __non_propagating_cache&
    operator=(const __non_propagating_cache& other)
    {
        if (std::addressof(other) != this) {
            if (other.has_) {
                other.value_.~T();
                other.has_ = false;
            }
        }

        return *this;
    };

    constexpr __non_propagating_cache&
    operator=(__non_propagating_cache&& other)
    {
        if (has_) {
            value_.~T();
            has_ = false;
        }

        if (other.has_) {
            other.value_.~T();
            other.has_ = false;
        }

        return *this;
    };

    constexpr T& operator*() noexcept
    {
        IRIS_ASSERT(has_);
        return value_;
    }

    constexpr const T& operator*() const noexcept
    {
        IRIS_ASSERT(has_);
        return value_;
    }

    template <typename... Args>
    constexpr T& emplace(Args&&... args)
    {
        if (has_) {
            value_.~T();
            has_ = false;
        }

        std::construct_at(&value_, std::forward<Args>(args)...);
        has_ = true;

        return value_;
    }

private:
    union {
        T value_;
    };

    bool has_ = false;
};

template <typename Fn, typename... Args>
class range_adaptor_closure
#if defined(_MSC_VER)
    : public std::ranges::_Pipe::_Base<range_adaptor_closure<Fn, Args...>>
#elif defined(__GNUC__) && __GNUC__ >= 11
    : public std::ranges::views::__adaptor::_Partial<
          range_adaptor_closure<Fn, Args...>>
#endif
{
public:
    template <typename... Args2>
    constexpr explicit range_adaptor_closure(Args2&&... args)
        : args_(std::forward<Args2>(args)...)
    {
    }

    void operator()(auto&&) & = delete;
    void operator()(auto&&) const& = delete;
    void operator()(auto&&) && = delete;
    void operator()(auto&&) const&& = delete;

    template <typename T>
        requires std::invocable<Fn, T, Args...>
    constexpr decltype(auto) operator()(T&& t) &
    {
        return invoke(*this, std::forward<T>(t),
                      std::index_sequence_for<Args...> {});
    }

    template <typename T>
        requires std::invocable<Fn, T, const Args...>
    constexpr decltype(auto) operator()(T&& t) const&
    {
        return invoke(*this, std::forward<T>(t),
                      std::index_sequence_for<Args...> {});
    }

    template <typename T>
        requires std::invocable<Fn, T, Args...>
    constexpr decltype(auto) operator()(T&& t) &&
    {
        return invoke(*this, std::forward<T>(t),
                      std::index_sequence_for<Args...> {});
    }

    template <typename T>
        requires std::invocable<Fn, T, const Args...>
    constexpr decltype(auto) operator()(T&& t) const&&
    {
        return invoke(*this, std::forward<T>(t),
                      std::index_sequence_for<Args...> {});
    }

private:
    template <typename Self, typename T, std::size_t... Indexes>
    static constexpr decltype(auto)
    invoke(Self&& self, T&& t, std::index_sequence<Indexes...>)
    {
        return Fn {}(std::forward<T>(t),
                     std::get<Indexes>(std::forward<Self>(self).args_)...);
    }

    std::tuple<Args...> args_;
};

}
