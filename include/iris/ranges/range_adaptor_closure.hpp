#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {
namespace __range_adaptor_closure_detail {
    template <typename T>
    using __pipeable_base =
#if defined(_MSC_VER)
        std::ranges::_Pipe::_Base<T>
#elif defined(__GNUC__) && __GNUC__ >= 11
        std::ranges::views::__adaptor::_Partial<T>
#endif
        ;
}

template <typename Fn, typename... Args>
class range_adaptor_closure
    : public __range_adaptor_closure_detail::__pipeable_base<
          range_adaptor_closure<Fn, Args...>> {
public:
    template <typename... Args2>
    constexpr explicit range_adaptor_closure(Args2&&... args)
        : args_(std::forward<Args2>(args)...)
    {
    }

    template <typename Range>
        requires std::invocable<Fn, Range, const Args&...>
    constexpr auto operator()(Range&& range) const&
    {
        return std::apply(
            [&](const auto&... args) {
                return Fn {}(std::forward<Range>(range), args...);
            },
            args_);
    }

    template <typename Range>
        requires std::invocable<Fn, Range, Args...>
    constexpr auto operator()(Range&& range) &&
    {
        return std::apply(
            [&](auto&... args) {
                return Fn {}(std::forward<Range>(range), std::move(args)...);
            },
            args_);
    }

private:
    std::tuple<Args...> args_;
};

template <typename Fn, typename Arg>
class range_adaptor_closure<Fn, Arg>
    : public __range_adaptor_closure_detail::__pipeable_base<
          range_adaptor_closure<Fn, Arg>> {
public:
    template <typename Arg2>
    constexpr explicit range_adaptor_closure(Arg2&& arg)
        : arg_(std::forward<Arg2>(arg))
    {
    }

    template <typename Range>
        requires std::invocable<Fn, Range, const Arg&>
    constexpr auto operator()(Range&& range) const&
    {
        return Fn {}(std::forward<Range>(range), arg_);
    }

    template <typename Range>
        requires std::invocable<Fn, Range, Arg>
    constexpr auto operator()(Range&& range) &&
    {
        return Fn {}(std::forward<Range>(range), std::move(arg_));
    }

private:
    Arg arg_;
};

template <typename Fn>
class range_adaptor_closure<Fn>
    : public __range_adaptor_closure_detail::__pipeable_base<
          range_adaptor_closure<Fn>> {
public:
    range_adaptor_closure() = default;

    template <typename Range>
        requires std::invocable<Fn, Range>
    constexpr auto operator()(Range&& r) const&
    {
        return Fn {}(std::forward<Range>(r));
    }

    template <typename Range>
        requires std::invocable<Fn, Range>
    constexpr auto operator()(Range&& r) &&
    {
        return Fn {}(std::forward<Range>(r));
    }
};

}
