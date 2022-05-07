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

    template <typename T, typename = void>
    inline constexpr bool __is_complete_v = false;

    template <typename T>
    inline constexpr bool
        __is_complete_v<T, std::void_t<decltype(sizeof(T))>> = true;
}

template <typename Fn>
class range_adaptor_closure
    : public __range_adaptor_closure_detail::__pipeable_base<
          range_adaptor_closure<Fn>> {
public:
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

template <typename Fn>
    requires __range_adaptor_closure_detail::__is_complete_v<Fn>
class range_adaptor_closure<Fn>
    : public Fn,
      public __range_adaptor_closure_detail::__pipeable_base<
          range_adaptor_closure<Fn>> {
public:
    constexpr explicit range_adaptor_closure(Fn&& fn)
        : Fn(std::move(fn))
    {
    }

    using Fn::operator();
};

template <typename Fn>
range_adaptor_closure(Fn&&) -> range_adaptor_closure<Fn>;

}
