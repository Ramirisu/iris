#pragma once

#include <iris/config.hpp>

#include <iris/type_traits.hpp>

#include <coroutine>
#include <type_traits>

namespace iris {
namespace __coroutine_detail {
    template <typename T>
    struct is_coroutine_handle : std::false_type {
    };

    template <typename Promise>
    struct is_coroutine_handle<std::coroutine_handle<Promise>>
        : std::true_type {
    };

    template <typename T>
    inline constexpr bool is_coroutine_handle_v = is_coroutine_handle<T>::value;

    template <typename T>
    concept suspend_result
        = std::is_void_v<T> || std::is_same_v<T,
                                              bool> || is_coroutine_handle_v<T>;

    template <typename T, typename Promise>
    concept simple_awaitable
        = requires(T& t, const std::coroutine_handle<Promise>& h)
    {
        // clang-format off
        { t.await_ready() } -> std::convertible_to<bool>;
        { t.await_suspend(h) } -> suspend_result;
        t.await_resume();
        // clang-format on
    };

    template <typename T, typename Promise>
    concept has_member_co_await = requires(T&& t)
    {
        // clang-format off
        { std::forward<T>(t).operator co_await() } -> simple_awaitable<Promise>;
        // clang-format on
    };

    template <typename T, typename Promise>
    concept has_adl_co_await = requires(T&& t)
    {
        // clang-format off
        { operator co_await(std::forward<T>(t)) } -> simple_awaitable<Promise>;
        // clang-format on
    };

    template <typename T>
    auto get_awaitable(T&& t)
    {
        if constexpr (simple_awaitable<T, void>) {
            return std::forward<T>(t);
        } else if constexpr (has_member_co_await<T, void>) {
            return std::forward<T>(t).operator co_await();
        } else if constexpr (has_adl_co_await<T, void>) {
            return operator co_await(std::forward<T>(t));
        } else {
            static_assert(always_false_v<T>);
        }
    }
}

template <typename T>
concept suspend_result = __coroutine_detail::suspend_result<T>;

template <typename T, typename Promise = void>
concept simple_awaitable = __coroutine_detail::simple_awaitable<T, Promise>;

template <typename T, typename Promise = void>
concept has_member_co_await
    = __coroutine_detail::has_member_co_await<T, Promise>;

template <typename T, typename Promise = void>
concept has_adl_co_await = __coroutine_detail::has_adl_co_await<T, Promise>;

template <typename T, typename Promise = void>
concept awaitable = has_member_co_await<T, Promise> || has_adl_co_await<
    T,
    Promise> || simple_awaitable<T, Promise>;

template <typename T>
struct awaitable_result {
    using type = decltype(__coroutine_detail::get_awaitable(std::declval<T>())
                              .await_resume());
};

template <typename T>
using awaitable_result_t = typename awaitable_result<T>::type;

}
