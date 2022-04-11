#pragma once

#include <iris/config.hpp>

#include <iris/detail/type_traits.hpp>

namespace iris {

template <typename T>
inline constexpr bool always_false_v = false;

template <typename T, template <typename...> class U>
struct is_specialized_of : std::false_type {
};

template <template <typename...> class U, typename... Args>
struct is_specialized_of<U<Args...>, U> : std::true_type {
};

template <typename T, template <typename...> class U>
inline constexpr bool is_specialized_of_v = is_specialized_of<T, U>::value;

}
