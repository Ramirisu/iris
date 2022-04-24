#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

#include <concepts>
#include <memory>
#include <utility>

namespace iris {
namespace __expected_detail {
    template <typename New, typename Old, typename... Args>
    constexpr void __reinit(New& newval, Old& oldval, Args&&... args)
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
    // clang-format off
    template <typename E2 = E>        
        requires(!std::is_same_v<std::remove_cvref_t<E2>, unexpected> 
            && !std::is_same_v<std::remove_cvref_t<E2>, std::in_place_t> 
            && std::is_constructible_v<E, E2>)
    // clang-format on
    constexpr explicit unexpected(E2&& error)
        : error_(std::forward<E2>(error))
    {
    }

    template <typename... Args>
        requires std::is_constructible_v<E, Args...>
    constexpr explicit unexpected(std::in_place_t, Args&&... args)
        : error_(std::forward<Args>(args)...)
    {
    }

    // clang-format off
    template <typename U, typename... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
    constexpr explicit unexpected(std::in_place_t,
                                  std::initializer_list<U> list,
                                  Args&&... args)
        : error_(list, std::forward<Args>(args)...)
    // clang-format on
    {
    }

    constexpr unexpected(const unexpected&) = default;

    constexpr unexpected(unexpected&&) = default;

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

    constexpr void swap(unexpected& other) noexcept(
        std::is_nothrow_swappable_v<E>) requires std::is_swappable_v<E>
    {
        using std::swap;
        swap(error_, other.error_);
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
        return lhs.error() == rhs.error();
    }

private:
    E error_;
};

template <typename E>
unexpected(E) -> unexpected<E>;

struct unexpect_t {
    explicit unexpect_t() = default;
};

template <typename E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
public:
    const char* what() const noexcept override
    {
        return "bad access to a expected with error.";
    }

protected:
    bad_expected_access() noexcept = default;

    bad_expected_access(const bad_expected_access&) = default;

    bad_expected_access(bad_expected_access&&) = default;

    bad_expected_access& operator=(const bad_expected_access&) = default;

    bad_expected_access& operator=(bad_expected_access&&) = default;

    ~bad_expected_access() = default;
};

template <typename E>
class bad_expected_access : public bad_expected_access<void> {
public:
    explicit bad_expected_access(E error)
        : error_(std::move(error))
    {
    }

    const char* what() const noexcept override
    {
        return "bad access to a expected with error.";
    }

    E& error() & noexcept
    {
        return error_;
    }

    const E& error() const& noexcept
    {
        return error_;
    }

    E&& error() && noexcept
    {
        return std::move(error_);
    }

    const E&& error() const&& noexcept
    {
        return std::move(error_);
    }

private:
    E error_;
};

inline constexpr unexpect_t unexpect {};

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

    template <typename T2>
    using rebind = expected<T2, error_type>;

    constexpr expected() requires std::is_default_constructible_v<T>
        : state_(state::has_value), value_()
    {
    }

    // clang-format off
    template <typename T2 = T>
        requires (!std::is_same_v<std::remove_cvref_t<T2>, std::in_place_t>
            && !std::is_same_v<std::remove_cvref_t<T2>, expected>
            && !is_specialization_of_v<std::remove_cvref_t<T2>, unexpected>
            && std::is_constructible_v<T, T2>)
    // clang-format on
    constexpr expected(T2&& value)
        : state_(state::has_value)
        , value_(std::forward<T2>(value))
    {
    }

    template <typename... Args>
        requires std::is_constructible_v<T, Args...>
    constexpr explicit expected(std::in_place_t, Args&&... args)
        : state_(state::has_value)
        , value_(std::forward<Args>(args)...)
    {
    }

    // clang-format off
    template <typename U, typename... Args>
        requires std::is_constructible_v<T, std::initializer_list<U>&, Args...>
    constexpr explicit expected(std::in_place_t,
                                std::initializer_list<U> list,
                                Args&&... args)
        : state_(state::has_value)
        , value_(list, std::forward<Args>(args)...)
    // clang-format on
    {
    }

    template <typename... Args>
        requires std::is_constructible_v<E, Args...>
    constexpr explicit expected(unexpect_t, Args&&... args)
        : state_(state::has_error)
        , error_(std::forward<Args>(args)...)
    {
    }

    // clang-format off
    template <typename U, typename... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
    constexpr explicit expected(unexpect_t,
                                std::initializer_list<U> list,
                                Args&&... args)
        : state_(state::has_error)
        , error_(list, std::forward<Args>(args)...)
    // clang-format on
    {
    }

    template <typename E2>
        requires std::is_constructible_v<E, const E2&>
    constexpr explicit(!std::is_convertible_v<const E2&, E>)
        expected(const unexpected<E2>& unexp)
        : state_(state::has_error)
        , error_(std::forward<const E2&>(unexp.error()))
    {
    }

    template <typename E2>
        requires std::is_constructible_v<E, E2>
    constexpr explicit(!std::is_convertible_v<E2, E>)
        expected(unexpected<E2>&& unexp)
        : state_(state::has_error)
        , error_(std::forward<E2>(unexp.error()))
    {
    }

    // clang-format off
    constexpr expected(const expected&) 
        requires (std::is_trivially_copy_constructible_v<T> 
            && std::is_trivially_copy_constructible_v<E>) = default;
    // clang-format on

    // clang-format off
    constexpr expected(const expected& other) 
        requires (std::is_copy_constructible_v<T> 
            && std::is_copy_constructible_v<E> 
            && !(std::is_trivially_copy_constructible_v<T> 
                && std::is_trivially_copy_constructible_v<E>))
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
    constexpr expected(expected&&) 
        requires (std::is_trivially_move_constructible_v<T> 
            && std::is_trivially_move_constructible_v<E>) = default;
    // clang-format on

    // clang-format off
    constexpr expected(expected&& other) 
        noexcept(std::is_nothrow_move_constructible_v<T>
            && std::is_nothrow_move_constructible_v<E>) 
        requires (std::is_move_constructible_v<T> 
            && std::is_move_constructible_v<E>
            && !(std::is_trivially_move_constructible_v<T> 
                && std::is_trivially_move_constructible_v<E>))
        : state_(other.state_)
    // clang-format on
    {
        if (has_value()) {
            std::construct_at(&value_, std::move(other.value()));
        } else {
            std::construct_at(&error_, std::move(other.error()));
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
    constexpr ~expected() 
        requires std::is_trivially_destructible_v<T>
        && std::is_trivially_destructible_v<E> = default;
    // clang-format on

    // clang-format off
    constexpr ~expected() 
        requires (!std::is_trivially_destructible_v<T> 
            || !std::is_trivially_destructible_v<E>)
    // clang-format on
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
                    __expected_detail::__reinit(error_, value_, other.error());
                }
            } else {
                if (other.has_value()) {
                    __expected_detail::__reinit(value_, error_, other.value());
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
                    __expected_detail::__reinit(error_, value_,
                                                std::move(other.error()));
                }
            } else {
                if (other.has_value()) {
                    __expected_detail::__reinit(value_, error_,
                                                std::move(other.value()));
                } else {
                    error_ = std::move(other.error());
                }
            }

            state_ = other.state_;
        }

        return *this;
    }

    // clang-format off
    template <class T2 = T>
    constexpr expected& operator=(T2&& value) 
        requires (!std::is_same_v<std::remove_cvref_t<T2>, expected> 
            && !is_specialization_of_v<std::remove_cvref_t<T2>, unexpected>
            && std::is_constructible_v<T, T2>
            && std::is_assignable_v<T&, T2>
            && (std::is_nothrow_constructible_v<T, T2> 
                || std::is_nothrow_move_constructible_v<T> 
                || std::is_nothrow_move_constructible_v<E>))
    // clang-format on
    {
        if (has_value()) {
            value_ = std::forward<T2>(value);
        } else {
            __expected_detail::__reinit(value_, error_,
                                        std::forward<T2>(value));
            state_ = state::has_value;
        }

        return *this;
    }

    // clang-format off
    template <class E2>
    constexpr expected& operator=(const unexpected<E2>& unexp)
        requires (std::is_constructible_v<E, const E2&>
            && std::is_assignable_v<E&, const E2&>
            && (std::is_nothrow_constructible_v<E, const E2&> 
                || std::is_nothrow_move_constructible_v<T> 
                || std::is_nothrow_move_constructible_v<E>))
    // clang-format on
    {
        if (has_value()) {
            __expected_detail::__reinit(error_, value_,
                                        std::forward<const E2&>(unexp.error()));
            state_ = state::has_error;
        } else {
            error_ = std::forward<const E2&>(unexp.error());
        }

        return *this;
    }

    // clang-format off
    template <class E2>
    constexpr expected& operator=(unexpected<E2>&& unexp)
        requires (std::is_constructible_v<E, E2>
            && std::is_assignable_v<E&, E2>
            && (std::is_nothrow_constructible_v<E, E2> 
                || std::is_nothrow_move_constructible_v<T> 
                || std::is_nothrow_move_constructible_v<E>))
    // clang-format on
    {
        if (has_value()) {
            __expected_detail::__reinit(error_, value_,
                                        std::forward<E2>(unexp.error()));
            state_ = state::has_error;
        } else {
            error_ = std::forward<E2>(unexp.error());
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
    template <class U, class... Args>
    constexpr T& emplace(std::initializer_list<U> list, Args&&... args) noexcept 
        requires std::is_nothrow_constructible_v<T, std::initializer_list<U>&, Args...>
    // clang-format on
    {
        if (has_value()) {
            std::destroy_at(std::addressof(value_));
        } else {
            std::destroy_at(std::addressof(error_));
            state_ = state::has_value;
        }
        return *std::construct_at(std::addressof(value_), list,
                                  std::forward<Args>(args)...);
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

    constexpr const T* operator->() const noexcept
    {
        IRIS_ASSERT(has_value());
        return std::addressof(value_);
    }

    constexpr T* operator->() noexcept
    {
        IRIS_ASSERT(has_value());
        return std::addressof(value_);
    }

    constexpr const T& operator*() const& noexcept
    {
        IRIS_ASSERT(has_value());
        return value_;
    }

    constexpr T& operator*() & noexcept
    {
        IRIS_ASSERT(has_value());
        return value_;
    }

    constexpr const T&& operator*() const&& noexcept
    {
        IRIS_ASSERT(has_value());
        return std::move(value_);
    }

    constexpr T&& operator*() && noexcept
    {
        IRIS_ASSERT(has_value());
        return std::move(value_);
    }

    constexpr const T& value() const&
    {
        if (!has_value()) {
            throw bad_expected_access(error());
        }

        return value_;
    }

    constexpr T& value() &
    {
        if (!has_value()) {
            throw bad_expected_access(error());
        }

        return value_;
    }

    constexpr const T&& value() const&&
    {
        if (!has_value()) {
            throw bad_expected_access(std::move(error()));
        }

        return std::move(value_);
    }

    constexpr T&& value() &&
    {
        if (!has_value()) {
            throw bad_expected_access(std::move(error()));
        }

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
        IRIS_ASSERT(!has_value());
        return error_;
    }

    constexpr E& error() & noexcept
    {
        IRIS_ASSERT(!has_value());
        return error_;
    }

    constexpr const E&& error() const&& noexcept
    {
        IRIS_ASSERT(!has_value());
        return std::move(error_);
    }

    constexpr E&& error() && noexcept
    {
        IRIS_ASSERT(!has_value());
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
        return lhs.has_value() && lhs.value() == rhs;
    }

    template <typename E2>
    friend constexpr bool operator==(const expected& lhs,
                                     const unexpected<E2>& rhs)
    {
        return !lhs.has_value() && lhs.error() == rhs.error();
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

    template <typename T2>
    using rebind = expected<T2, error_type>;

    constexpr expected() noexcept
        : state_(state::has_value)
        , dummy_()
    {
    }

    template <typename E2>
        requires std::is_constructible_v<E, const E2&>
    constexpr explicit(!std::is_convertible_v<const E2&, E>)
        expected(const unexpected<E2>& unexp)
        : state_(state::has_error)
        , error_(std::forward<const E2&>(unexp.error()))
    {
    }

    template <typename E2>
        requires std::is_constructible_v<E, E2>
    constexpr explicit(!std::is_convertible_v<E2, E>)
        expected(unexpected<E2>&& unexp)
        : state_(state::has_error)
        , error_(std::forward<E2>(unexp.error()))
    {
    }

    constexpr expected(const expected&) //
        requires std::is_trivially_copy_constructible_v<E>
    = default;

    // clang-format off
    constexpr expected(const expected& other) 
        requires (std::is_copy_constructible_v<E> 
            && !std::is_trivially_copy_constructible_v<E>)
        : state_(other.state_)
    // clang-format on
    {
        if (!has_value()) {
            std::construct_at(&error_, other.error());
        }
    }

    constexpr expected(expected&&) //
        requires std::is_trivially_move_constructible_v<E>
    = default;

    // clang-format off
    constexpr expected(expected&& other) 
        noexcept(std::is_nothrow_move_constructible_v<E>) 
        requires (std::is_move_constructible_v<E> 
            && !std::is_trivially_move_constructible_v<E>)
        : state_(other.state_)
    // clang-format on
    {
        if (!has_value()) {
            std::construct_at(&error_, std::move(other.error()));
        }
    }

    constexpr explicit expected(std::in_place_t) noexcept
        : state_(state::has_value)
    {
    }

    template <typename... Args>
        requires std::is_constructible_v<E, Args...>
    constexpr explicit expected(unexpect_t, Args&&... args)
        : state_(state::has_error)
        , error_(std::forward<Args>(args)...)
    {
    }

    // clang-format off
    template <typename U, typename... Args>
        requires std::is_constructible_v<E, std::initializer_list<U>&, Args...>
        constexpr explicit expected(unexpect_t, 
                                    std::initializer_list<U> list, 
                                    Args&&... args)
        : state_(state::has_error)
        , error_(list, std::forward<Args>(args)...)
    // clang-format on
    {
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

    constexpr ~expected() requires std::is_trivially_destructible_v<E>
    = default;

    constexpr ~expected() requires(!std::is_trivially_destructible_v<E>)
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

    // clang-format off
    template <class E2>
    constexpr expected& operator=(const unexpected<E2>& unexp)
        requires (std::is_constructible_v<E, const E2&>
            && std::is_assignable_v<E&, const E2&>)
    // clang-format on
    {
        if (has_value()) {
            std::construct_at(std::addressof(error_),
                              std::forward<const E2&>(unexp.error()));
            state_ = state::has_error;
        } else {
            error_ = std::forward<const E2&>(unexp.error());
        }

        return *this;
    }

    // clang-format off
    template <class E2>
    constexpr expected& operator=(unexpected<E2>&& unexp)
        requires (std::is_constructible_v<E, E2>
            && std::is_assignable_v<E&, E2>)
    // clang-format on
    {
        if (has_value()) {
            std::construct_at(std::addressof(error_),
                              std::forward<E2>(unexp.error()));
            state_ = state::has_error;
        } else {
            error_ = std::forward<E2>(unexp.error());
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

    constexpr void operator*() const noexcept
    {
        IRIS_ASSERT(has_value());
    }

    constexpr const void value() const&
    {
        if (!has_value()) {
            throw bad_expected_access(error());
        }
    }

    constexpr void value() &&
    {
        if (!has_value()) {
            throw bad_expected_access(error());
        }
    }

    constexpr const E& error() const& noexcept
    {
        IRIS_ASSERT(!has_value());
        return error_;
    }

    constexpr E& error() & noexcept
    {
        IRIS_ASSERT(!has_value());
        return error_;
    }

    constexpr const E&& error() const&& noexcept
    {
        IRIS_ASSERT(!has_value());
        return std::move(error_);
    }

    constexpr E&& error() && noexcept
    {
        IRIS_ASSERT(!has_value());
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
        return !lhs.has_value() && lhs.error() == rhs.error();
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
