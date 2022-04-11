#pragma once

#include <iris/config.hpp>

#include <tuple>

namespace iris {
namespace __bind_detail {

    template <typename Fn, typename... Args>
    class bind_front {
    public:
        template <typename Fn2, typename... Args2>
            requires(!std::same_as<std::remove_cvref_t<Fn2>, bind_front>)
        constexpr explicit bind_front(Fn2&& fn, Args2&&... args)
            : fn_(std::forward<Fn2>(fn))
            , bound_(std::forward<Args2>(args)...)
        {
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) &
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(fn_,
                                       std::forward<decltype(bound)>(bound)...,
                                       std::forward<CallArgs>(args)...);
                },
                bound_);
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) const&
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(fn_,
                                       std::forward<decltype(bound)>(bound)...,
                                       std::forward<CallArgs>(args)...);
                },
                bound_);
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) &&
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(std::move(fn_),
                                       std::forward<decltype(bound)>(bound)...,
                                       std::forward<CallArgs>(args)...);
                },
                std::move(bound_));
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) const&&
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(std::move(fn_),
                                       std::forward<decltype(bound)>(bound)...,
                                       std::forward<CallArgs>(args)...);
                },
                std::move(bound_));
        }

    private:
        Fn fn_;
        std::tuple<Args...> bound_;
    };

    template <typename Fn, typename... Args>
    class bind_back {
    public:
        template <typename Fn2, typename... Args2>
            requires(!std::same_as<std::remove_cvref_t<Fn2>, bind_back>)
        constexpr explicit bind_back(Fn2&& fn, Args2&&... args)
            : fn_(std::forward<Fn2>(fn))
            , bound_(std::forward<Args2>(args)...)
        {
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) &
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(fn_, std::forward<CallArgs>(args)...,
                                       std::forward<decltype(bound)>(bound)...);
                },
                bound_);
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) const&
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(fn_, std::forward<CallArgs>(args)...,
                                       std::forward<decltype(bound)>(bound)...);
                },
                bound_);
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) &&
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(std::move(fn_),
                                       std::forward<CallArgs>(args)...,
                                       std::forward<decltype(bound)>(bound)...);
                },
                std::move(bound_));
        }

        template <typename... CallArgs>
        constexpr auto operator()(CallArgs&&... args) const&&
        {
            return std::apply(
                [&](auto&&... bound) {
                    return std::invoke(std::move(fn_),
                                       std::forward<CallArgs>(args)...,
                                       std::forward<decltype(bound)>(bound)...);
                },
                std::move(bound_));
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

    return __bind_detail::bind_front<std::decay_t<Fn>, std::decay_t<Args>...>(
        std::forward<Fn>(fn), std::forward<Args>(args)...);
}

template <typename Fn, typename... Args>
constexpr auto bind_back(Fn&& fn, Args&&... args)
{
    static_assert(std::is_constructible_v<std::decay_t<Fn>, Fn>);
    static_assert(std::is_move_constructible_v<std::decay_t<Fn>>);
    static_assert((std::is_constructible_v<std::decay_t<Args>, Args> && ...));
    static_assert((std::is_move_constructible_v<std::decay_t<Args>> && ...));

    return __bind_detail::bind_back<std::decay_t<Fn>, std::decay_t<Args>...>(
        std::forward<Fn>(fn), std::forward<Args>(args)...);
}

}
