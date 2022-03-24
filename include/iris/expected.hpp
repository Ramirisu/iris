#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

#include <concepts>
#include <utility>

namespace iris {
namespace detail {
    template <typename New, typename Old, typename... Args>
    constexpr void expected_reinit(New& newval, Old& oldval, Args&&... args)
    {
        if constexpr (std::is_nothrow_constructible_v<New, Args...>) {
            std::destroy_at(&oldval);
            std::construct_at(&newval, std::forward<Args>(args)...);
        } else if constexpr (std::is_nothrow_move_constructible_v<New>) {
            New tmp(std::forward<Args>(args)...);
            std::destroy_at(&oldval);
            std::construct_at(&newval, std::move(tmp));
        } else {
            Old tmp(std::move(oldval));
            std::destroy_at(&oldval);
            try {
                std::construct_at(&newval, std::forward<Args>(args)...);
            } catch (...) {
                std::construct_at(&oldval, std::move(tmp));
                throw;
            }
        }
    }
}

template <typename E>
class unexpected {
public:
    template <typename E2>
        requires std::is_convertible_v<E2, E>
    constexpr explicit unexpected(E2&& e)
        : value_(std::forward<E2>(e))
    {
    }

    constexpr const E& value() const& noexcept
    {
        return value_;
    }

    constexpr E& value() & noexcept
    {
        return value_;
    }

    constexpr const E&& value() const&& noexcept
    {
        return std::move(value_);
    }

    constexpr E&& value() && noexcept
    {
        return std::move(value_);
    }

    constexpr void swap(unexpected& other) noexcept(
        std::is_nothrow_swappable_v<E>) requires std::is_swappable_v<E>
    {
        using std::swap;
        swap(value_, other.value_);
    }

    friend constexpr void swap(unexpected& lhs, unexpected& rhs) noexcept(
        noexcept(lhs.swap(rhs))) requires std::is_swappable_v<E>
    {
        lhs.swap(rhs);
    }

    template <class E2>
    friend constexpr bool operator==(const unexpected& lhs,
                                     const unexpected<E2>& rhs)
    {
        return lhs.value() == rhs.value();
    }

private:
    E value_;
};

template <typename E>
unexpected(E) -> unexpected<E>;

template <typename T, typename E>
class expected {
    enum class state {
        has_value,
        has_error,
    };

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    constexpr expected() requires std::is_default_constructible_v<T>
        : state_(state::has_value), value_()
    {
    }

    // clang-format off
    template <typename T2>
        requires (std::is_constructible_v<T, T2>
            && !std::is_same_v<T2, expected>
            && !std::is_same_v<T2, unexpected<E>>)
    // clang-format on
    constexpr expected(T2&& value)
        : state_(state::has_value)
        , value_(std::forward<T2>(value))
    {
    }

    template <typename E2>
        requires std::is_constructible_v<E, E2>
    constexpr expected(unexpected<E2>&& unexp)
        : state_(state::has_error)
        , error_(std::forward<E2>(unexp.value()))
    {
    }

    // clang-format off
    constexpr expected(const expected& other) 
        requires (std::is_copy_constructible_v<T> 
            && std::is_copy_constructible_v<E>)
        : state_(other.state_)
    // clang-format on
    {
        if (has_value()) {
            std::construct_at(&value_, other.value());
        } else {
            std::construct_at(&error_, other.error());
        }
    }

    // clang-format off
    constexpr expected(expected&& other) 
        noexcept(std::is_nothrow_move_constructible_v<T>
            && std::is_nothrow_move_constructible_v<E>) 
        requires (std::is_move_constructible_v<T> 
            && std::is_move_constructible_v<E>)
        : state_(other.state_)
    // clang-format on
    {
        if (has_value()) {
            std::construct_at(&value_, std::move(other.value()));
        } else {
            std::construct_at(&error_, std::move(other.error()));
        }
    }

    constexpr ~expected()
    {
        switch (state_) {
        case state::has_value:
            std::destroy_at(&value_);
            break;
        case state::has_error:
            std::destroy_at(&error_);
            break;
        }
    }

    // clang-format off
    template <typename T2, typename E2>
    constexpr explicit(!std::is_convertible_v<const T2&, T> || !std::is_convertible_v<const E2&, E>) 
        expected(const expected<T2, E2>& other) 
        requires (std::is_constructible_v<T, const T2&>
            && std::is_constructible_v<E, const E2&> 
            && !std::is_constructible_v<T, expected<T2, E2>&> 
            && !std::is_constructible_v<T, expected<T2, E2>> 
            && !std::is_constructible_v<T, const expected<T2, E2>&>
            && !std::is_constructible_v<T, const expected<T2, E2>&>
            && !std::is_convertible_v<expected<T2, E2>&, T>
            && !std::is_convertible_v<expected<T2, E2>&&, T>
            && !std::is_convertible_v<const expected<T2, E2>&, T>
            && !std::is_convertible_v<const expected<T2, E2>&&, T>
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>&> 
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>> 
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>)
    // clang-format on
    {
        if (other.has_value()) {
            std::construct_at(&value_, std::forward<const T2&>(other.value()));
            state_ = state::has_value;
        } else {
            std::construct_at(&error_, std::forward<const E2&>(other.error()));
            state_ = state::has_error;
        }
    }

    // clang-format off
    template <typename T2, typename E2>
    constexpr explicit(!std::is_convertible_v<T2, T> || !std::is_convertible_v<E2, E>) 
        expected(expected<T2, E2>&& other) 
        requires (std::is_constructible_v<T, T2>
            && std::is_constructible_v<E, E2> 
            && !std::is_constructible_v<T, expected<T2, E2>&> 
            && !std::is_constructible_v<T, expected<T2, E2>> 
            && !std::is_constructible_v<T, const expected<T2, E2>&>
            && !std::is_constructible_v<T, const expected<T2, E2>&>
            && !std::is_convertible_v<expected<T2, E2>&, T>
            && !std::is_convertible_v<expected<T2, E2>&&, T>
            && !std::is_convertible_v<const expected<T2, E2>&, T>
            && !std::is_convertible_v<const expected<T2, E2>&&, T>
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>&> 
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>> 
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>)
    // clang-format on
    {
        if (other.has_value()) {
            std::construct_at(&value_, std::forward<T2>(other.value()));
            state_ = state::has_value;
        } else {
            std::construct_at(&error_, std::forward<E2>(other.error()));
            state_ = state::has_error;
        }
    }

    // clang-format off
    constexpr expected& operator=(const expected& other)
        requires (std::is_copy_constructible_v<T> 
            && std::is_copy_constructible_v<E> 
            && std::is_copy_assignable_v<T> 
            && std::is_copy_assignable_v<E> 
            && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>))
    // clang-format on
    {
        if (this != &other) {
            if (has_value()) {
                if (other.has_value()) {
                    value_ = other.value();
                } else {
                    detail::expected_reinit(error_, value_, other.error());
                }
            } else {
                if (other.has_value()) {
                    detail::expected_reinit(value_, error_, other.value());
                } else {
                    error_ = other.error();
                }
            }

            state_ = other.state_;
        }

        return *this;
    }

    // clang-format off
    constexpr expected& operator=(expected&& other)
        noexcept(std::is_nothrow_move_constructible_v<T>
            && std::is_nothrow_move_constructible_v<E>
            && std::is_nothrow_move_assignable_v<T>
            && std::is_nothrow_move_assignable_v<E>)
        requires std::is_move_constructible_v<T> 
            && std::is_move_constructible_v<E> 
            && std::is_move_assignable_v<T>
            && std::is_move_assignable_v<E>
            && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>)
    // clang-format on
    {
        if (this != &other) {
            if (has_value()) {
                if (other.has_value()) {
                    value_ = std::move(other.value());
                } else {
                    detail::expected_reinit(error_, value_,
                                            std::move(other.error()));
                }
            } else {
                if (other.has_value()) {
                    detail::expected_reinit(value_, error_,
                                            std::move(other.value()));
                } else {
                    error_ = std::move(other.error());
                }
            }

            state_ = other.state_;
        }

        return *this;
    }

    template <typename... Args>
    constexpr T& emplace(Args&&... args) noexcept requires
        std::is_nothrow_constructible_v<T, Args...>
    {
        if (has_value()) {
            std::destroy_at(&value_);
        } else {
            std::destroy_at(&error_);
            state_ = state::has_value;
        }

        return *std::construct_at(&value_, std::forward<Args>(args)...);
    }

    // clang-format off
    constexpr void swap(expected& other)
        noexcept(std::is_nothrow_swappable_v<T> 
            && std::is_nothrow_swappable_v<E>
            && std::is_nothrow_move_constructible_v<T>
            && std::is_nothrow_move_constructible_v<E>)
        requires (std::is_swappable_v<T> 
            && std::is_swappable_v<E> 
            && std::is_move_constructible_v<T> 
            && std::is_move_constructible_v<E> 
            && (std::is_nothrow_move_constructible_v<T> || std::is_nothrow_move_constructible_v<E>))
    // clang-format on
    {
        if (has_value()) {
            if (other.has_value()) {
                using std::swap;
                swap(value_, other.value_);
            } else {
                if constexpr (
                    std::is_nothrow_move_constructible_v<
                        T> && std::is_nothrow_move_constructible_v<E>) {
                    E tmp(std::move(other.error_));
                    std::destroy_at(&other.error_);
                    std::construct_at(&other.value_, std::move(value_));
                    std::destroy_at(&value_);
                    std::construct_at(&error_, std::move(tmp));
                } else if constexpr (std::is_nothrow_move_constructible_v<E>) {
                    E tmp(std::move(other.error_));
                    std::destroy_at(&other.error_);
                    try {
                        std::construct_at(&other.value_, std::move(value_));
                        std::destroy_at(&value_);
                        std::construct_at(&error_, std::move(tmp));
                    } catch (...) {
                        std::construct_at(&other.error_, std::move(tmp));
                        throw;
                    }
                } else {
                    T tmp(std::move(value_));
                    std::destroy_at(&value_);
                    try {
                        std::construct_at(&error_, std::move(other.error_));
                        std::destroy_at(&other.error_);
                        std::construct_at(&other.value_, std::move(tmp));
                    } catch (...) {
                        std::construct_at(&value_, std::move(tmp));
                        throw;
                    }
                }
                state_ = state::has_error;
                other.state_ = state::has_value;
            }
        } else {
            if (other.has_value()) {
                other.swap(*this);
            } else {
                using std::swap;
                swap(error_, other.error_);
            }
        }
    }

    friend constexpr void swap(expected& lhs,
                               expected& rhs) noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }

    constexpr explicit operator bool() const noexcept
    {
        return state_ == state::has_value;
    }

    constexpr bool has_value() const noexcept
    {
        return static_cast<bool>(*this);
    }

    constexpr const T& value() const& noexcept
    {
        return value_;
    }

    constexpr T& value() & noexcept
    {
        return value_;
    }

    constexpr const T&& value() const&& noexcept
    {
        return std::move(value_);
    }

    constexpr T&& value() && noexcept
    {
        return std::move(value_);
    }

    // clang-format off
    template <typename U>
    constexpr T value_or(U&& u) const&
        requires (std::is_copy_constructible_v<T> && std::is_convertible_v<U, T>)
    // clang-format on
    {
        return has_value() ? value() : static_cast<T>(std::forward<U>(u));
    }

    // clang-format off
    template <typename U>
    constexpr T value_or(U&& u) &&
        requires (std::is_move_constructible_v<T> && std::is_convertible_v<U, T>)
    // clang-format on
    {
        return has_value() ? value() : static_cast<T>(std::forward<U>(u));
    }

    constexpr const E& error() const& noexcept
    {
        return error_;
    }

    constexpr E& error() & noexcept
    {
        return error_;
    }

    constexpr const E&& error() const&& noexcept
    {
        return std::move(error_);
    }

    constexpr E&& error() && noexcept
    {
        return std::move(error_);
    }

    // clang-format off
    template <typename T2, typename E2>
    friend constexpr bool  operator==(const expected& lhs, const expected<T2, E2>& rhs) 
        requires (!std::is_void_v<T2>)
    // clang-format on
    {
        if (lhs.has_value() != rhs.has_value()) {
            return false;
        }

        return lhs.has_value() ? lhs.value() == rhs.value()
                               : lhs.error() == rhs.error();
    }

    template <typename T2>
    friend constexpr bool operator==(const expected& lhs, const T2& rhs)
    {
        return lhs.value() == rhs;
    }

    template <typename E2>
    friend constexpr bool operator==(const expected& lhs,
                                     const unexpected<E2>& rhs)
    {
        return !lhs.has_value() && lhs.error() == rhs.value();
    }

    // clang-format off
private:
    // clang-format on
    state state_;
    union {
        T value_;
        E error_;
    };
};

template <typename E>
class expected<void, E> {
    enum class state {
        has_value,
        has_error,
    };

public:
    using value_type = void;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    constexpr expected() noexcept
        : state_(state::has_value)
        , dummy_()
    {
    }

    template <typename E2>
        requires std::is_constructible_v<E, E2>
    constexpr expected(unexpected<E2>&& unexp)
        : state_(state::has_error)
        , error_(std::forward<E2>(unexp.value()))
    {
    }

    constexpr expected(
        const expected& other) requires std::is_copy_constructible_v<E>
        : state_(other.state_)
    {
        if (!has_value()) {
            std::construct_at(&error_, other.error());
        }
    }

    // clang-format off
    constexpr expected(expected&& other) 
        noexcept(std::is_nothrow_move_constructible_v<E>) 
        requires std::is_move_constructible_v<E> 
        : state_(other.state_)
    // clang-format on
    {
        if (!has_value()) {
            std::construct_at(&error_, std::move(other.error()));
        }
    }

    constexpr ~expected()
    {
        switch (state_) {
        case state::has_value:
            break;
        case state::has_error:
            std::destroy_at(&error_);
            break;
        }
    }

    // clang-format off
    template <typename T2, typename E2>
    constexpr explicit(!std::is_convertible_v<const E2&, E>) 
        expected(const expected<T2, E2>& other) 
        requires (std::is_void_v<T2>
            && std::is_constructible_v<E, const E2&> 
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>&> 
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>> 
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>)
    // clang-format on
    {
        if (other.has_value()) {
            state_ = state::has_value;
        } else {
            std::construct_at(&error_, std::forward<const E2&>(other.error()));
            state_ = state::has_error;
        }
    }

    // clang-format off
    template <typename T2, typename E2>
    constexpr explicit(!std::is_convertible_v<E2, E>) 
        expected(expected<T2, E2>&& other) 
        requires (std::is_void_v<T2>
            && std::is_constructible_v<E, E2> 
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>&> 
            && !std::is_constructible_v<unexpected<E>, expected<T2, E2>> 
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>
            && !std::is_constructible_v<unexpected<E>, const expected<T2, E2>&>)
    // clang-format on
    {
        if (other.has_value()) {
            state_ = state::has_value;
        } else {
            std::construct_at(&error_, std::forward<E2>(other.error()));
            state_ = state::has_error;
        }
    }

    // clang-format off
    constexpr expected& operator=(const expected& other) 
        requires (std::is_copy_assignable_v<E>
            && std::is_copy_constructible_v<E>)
    // clang-format on
    {
        if (this != &other) {
            if (has_value()) {
                if (!other.has_value()) {
                    std::construct_at(&error_, other.error());
                }
            } else {
                if (other.has_value()) {
                    std::destroy_at(&other.error_);
                } else {
                    error_ = other.error();
                }
            }

            state_ = other.state_;
        }

        return *this;
    }

    // clang-format off
    constexpr expected& operator=(expected&& other)
        noexcept(std::is_nothrow_move_constructible_v<E>
            && std::is_nothrow_move_assignable_v<E>)
        requires (std::is_move_constructible_v<E> 
            && std::is_move_assignable_v<E>)
    // clang-format on
    {
        if (this != &other) {
            if (has_value()) {
                if (!other.has_value()) {
                    std::construct_at(&error_, std::move(other.error()));
                }
            } else {
                if (other.has_value()) {
                    std::destroy_at(&error_);
                } else {
                    error_ = std::move(other.error());
                }
            }

            state_ = other.state_;
        }

        return *this;
    }

    constexpr void emplace() noexcept
    {
        if (!has_value()) {
            std::destroy_at(&error_);
            state_ = state::has_value;
        }
    }

    // clang-format off
    constexpr void swap(expected& other)
        noexcept(std::is_nothrow_swappable_v<E>
            && std::is_nothrow_move_constructible_v<E>)
        requires (std::is_swappable_v<E> 
            && std::is_nothrow_move_constructible_v<E>)
    // clang-format on
    {
        if (has_value()) {
            if (!other.has_value()) {
                std::construct_at(&error_, std::move(other.error_));
                std::destroy_at(&other.error_);
                state_ = state::has_error;
                other.state_ = state::has_value;
            }
        } else {
            if (other.has_value()) {
                other.swap(*this);
            } else {
                using std::swap;
                swap(error_, other.error_);
            }
        }
    }

    friend constexpr void swap(expected& lhs,
                               expected& rhs) noexcept(noexcept(lhs.swap(rhs)))
    {
        lhs.swap(rhs);
    }

    constexpr explicit operator bool() const noexcept
    {
        return state_ == state::has_value;
    }

    constexpr bool has_value() const noexcept
    {
        return static_cast<bool>(*this);
    }

    constexpr const void value() const& noexcept { }

    constexpr void value() && noexcept { }

    constexpr const E& error() const& noexcept
    {
        return error_;
    }

    constexpr E& error() & noexcept
    {
        return error_;
    }

    constexpr const E&& error() const&& noexcept
    {
        return std::move(error_);
    }

    constexpr E&& error() && noexcept
    {
        return std::move(error_);
    }

    // clang-format off
    template <typename T2, typename E2>
    friend constexpr bool  operator==(const expected& lhs, const expected<T2, E2>& rhs) 
        requires std::is_void_v<T2>
    // clang-format on
    {
        if (lhs.has_value() != rhs.has_value()) {
            return false;
        }

        return lhs.has_value() || lhs.error() == rhs.error();
    }

    template <typename E2>
    friend constexpr bool operator==(const expected& lhs,
                                     const unexpected<E2>& rhs)

    {
        return !lhs.has_value() && lhs.error() == rhs.value();
    }

    // clang-format off
private:
    // clang-format on
    state state_;
    union {
        std::uint8_t dummy_;
        E error_;
    };
};

}
