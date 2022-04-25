#pragma once

#include <iris/config.hpp>

#include <iris/ranges/range_adaptor_closure.hpp>

namespace iris::ranges {
namespace views {
    class __unwrap_fn : public range_adaptor_closure<__unwrap_fn> {
    public:
        constexpr __unwrap_fn() noexcept = default;

        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
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
