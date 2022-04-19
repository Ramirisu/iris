#pragma once

#include <iris/config.hpp>

#include <ranges>

namespace iris::ranges::__detail {

template <typename T>
#if defined(_MSC_VER)
using __copyable_box = std::ranges::_Copyable_box<T>;
#elif defined(__GNUC__)
class __copyable_box : public std::ranges::__detail::__box<T> {
public:
    // gcc's impl doesn't have this...
    template <typename... Args>
    __copyable_box(std::in_place_t, Args&&... args)
        : std::ranges::__detail::__box<T>(std::forward<Args>(args)...)
    {
    }

    // gcc's impl doesn't have this...
    constexpr operator bool() const noexcept
    {
        return this->has_value();
    }
};
#else
#error "Unsupported compiler."
#endif

}
