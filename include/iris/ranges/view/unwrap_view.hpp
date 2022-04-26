#pragma once

#include <iris/config.hpp>

#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {
namespace __unwrap_detail {
    template <typename T>
    concept __has_member_value = requires(T t)
    {
        t.value();
    };
}

namespace views {
    class __unwrap_fn : public range_adaptor_closure<__unwrap_fn> {
    public:
        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const requires
            __unwrap_detail::__has_member_value<
                std::ranges::range_value_t<Range>>
        {
            return std::views::transform(
                std::forward<Range>(range), [](auto&& exp) {
                    return std::forward<decltype(exp)>(exp).value();
                });
        }
    };

    inline constexpr __unwrap_fn unwrap {};
}
}

namespace iris {
namespace views = ranges::views;
}
