#pragma once

#include <iris/config.hpp>

#include <functional>
#include <tuple>
#include <type_traits>

namespace iris {
namespace __bind_detail {

    template <typename Fn, typename... Args>
    class __bind_front {
    public:
        template <typename Fn2, typename... Args2>
            requires(!std::same_as<std::remove_cvref_t<Fn2>, __bind_front>)
        constexpr explicit __bind_front(Fn2&& fn, Args2&&... args)
            : fn_(std::forward<Fn2>(fn))
            , bound_(std::forward<Args2>(args)...)
        {
        }

        template <typename... CallArgs>
            requires std::invocable<Fn&, CallArgs..., Args&...>
        constexpr auto operator()(CallArgs&&... args) & noexcept(
            std::is_nothrow_invocable_v<Fn&, Args&..., CallArgs...>)
        {
            return std::apply(
                [&](auto&... bound) {
                    return std::invoke(fn_, bound...,
                                       std::forward<CallArgs>(args)...);
                },
                bound_);
        }

        template <typename... CallArgs>
            requires std::invocable<const Fn&, CallArgs..., const Args&...>
        constexpr auto operator()(CallArgs&&... args) const& noexcept(
            std::is_nothrow_invocable_v<const Fn&, const Args&..., CallArgs...>)
        {
            return std::apply(
                [&](const auto&... bound) {
                    return std::invoke(fn_, bound...,
                                       std::forward<CallArgs>(args)...);
                },
                bound_);
        }

        template <typename... CallArgs>
            requires std::invocable<Fn, CallArgs..., Args...>
        constexpr auto operator()(CallArgs&&... args) && noexcept(
            std::is_nothrow_invocable_v<Fn, Args..., CallArgs...>)
        {
            return std::apply(
                [&](auto&... bound) {
                    return std::invoke(std::move(fn_), std::move(bound)...,
                                       std::forward<CallArgs>(args)...);
                },
                bound_);
        }

        template <typename... CallArgs>
            requires std::invocable<const Fn, CallArgs..., const Args...>
        constexpr auto operator()(CallArgs&&... args) const&& noexcept(
            std::is_nothrow_invocable_v<const Fn, const Args..., CallArgs...>)
        {
            return std::apply(
                [&](const auto&... bound) {
                    return std::invoke(std::move(fn_), std::move(bound)...,
                                       std::forward<CallArgs>(args)...);
                },
                bound_);
        }

    private:
        Fn fn_;
        std::tuple<Args...> bound_;
    };

    template <typename Fn, typename... Args>
    class __bind_back {
    public:
        template <typename Fn2, typename... Args2>
            requires(!std::same_as<std::remove_cvref_t<Fn2>, __bind_back>)
        constexpr explicit __bind_back(Fn2&& fn, Args2&&... args)
            : fn_(std::forward<Fn2>(fn))
            , bound_(std::forward<Args2>(args)...)
        {
        }

        template <typename... CallArgs>
            requires std::invocable<Fn&, CallArgs..., Args&...>
        constexpr auto operator()(CallArgs&&... args) & noexcept(
            std::is_nothrow_invocable_v<Fn&, CallArgs..., Args&...>)
        {
            return std::apply(
                [&](auto&... bound) {
                    return std::invoke(fn_, std::forward<CallArgs>(args)...,
                                       bound...);
                },
                bound_);
        }

        template <typename... CallArgs>
            requires std::invocable<const Fn&, CallArgs..., const Args&...>
        constexpr auto operator()(CallArgs&&... args) const& noexcept(
            std::is_nothrow_invocable_v<const Fn&, CallArgs..., const Args&...>)
        {
            return std::apply(
                [&](const auto&... bound) {
                    return std::invoke(fn_, std::forward<CallArgs>(args)...,
                                       bound...);
                },
                bound_);
        }

        template <typename... CallArgs>
            requires std::invocable<Fn, CallArgs..., Args...>
        constexpr auto operator()(CallArgs&&... args) && noexcept(
            std::is_nothrow_invocable_v<Fn, CallArgs..., Args...>)
        {
            return std::apply(
                [&](auto&... bound) {
                    return std::invoke(std::move(fn_),
                                       std::forward<CallArgs>(args)...,
                                       std::move(bound)...);
                },
                bound_);
        }

        template <typename... CallArgs>
            requires std::invocable<const Fn, CallArgs..., const Args...>
        constexpr auto operator()(CallArgs&&... args) const&& noexcept(
            std::is_nothrow_invocable_v<const Fn, CallArgs..., const Args...>)
        {
            return std::apply(
                [&](const auto&... bound) {
                    return std::invoke(std::move(fn_),
                                       std::forward<CallArgs>(args)...,
                                       std::move(bound)...);
                },
                bound_);
        }

    private:
        Fn fn_;
        std::tuple<Args...> bound_;
    };

}

template <typename Fn, typename... Args>
constexpr auto bind_front(Fn&& fn, Args&&... args)
{
    static_assert(std::is_constructible_v<std::decay_t<Fn>, Fn>);
    static_assert(std::is_move_constructible_v<std::decay_t<Fn>>);
    static_assert((std::is_constructible_v<std::decay_t<Args>, Args> && ...));
    static_assert((std::is_move_constructible_v<std::decay_t<Args>> && ...));

    return __bind_detail::__bind_front<std::decay_t<Fn>, std::decay_t<Args>...>(
        std::forward<Fn>(fn), std::forward<Args>(args)...);
}

template <typename Fn, typename... Args>
constexpr auto bind_back(Fn&& fn, Args&&... args)
{
    static_assert(std::is_constructible_v<std::decay_t<Fn>, Fn>);
    static_assert(std::is_move_constructible_v<std::decay_t<Fn>>);
    static_assert((std::is_constructible_v<std::decay_t<Args>, Args> && ...));
    static_assert((std::is_move_constructible_v<std::decay_t<Args>> && ...));

    return __bind_detail::__bind_back<std::decay_t<Fn>, std::decay_t<Args>...>(
        std::forward<Fn>(fn), std::forward<Args>(args)...);
}

}
