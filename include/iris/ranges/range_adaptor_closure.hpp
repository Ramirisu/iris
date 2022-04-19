#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges {

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
