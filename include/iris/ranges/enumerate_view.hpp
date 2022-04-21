#pragma once

#include <iris/config.hpp>

#include <iris/ranges/range_adaptor_closure.hpp>
#include <iris/ranges/zip_view.hpp>

namespace iris::ranges {
namespace views {

    class __enumerate_fn : public range_adaptor_closure<__enumerate_fn> {
    public:
        constexpr __enumerate_fn() noexcept = default;

        template <std::ranges::viewable_range Range>
        constexpr auto operator()(Range&& range) const
        {
            return zip(std::views::iota(std::size_t(0)),
                       std::forward<Range>(range));
        }
    };

    inline constexpr __enumerate_fn enumerate {};
}
}

namespace iris {
namespace views = ranges::views;
}
