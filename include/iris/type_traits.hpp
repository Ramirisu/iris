#pragma once

#include <iris/config.hpp>

#include <iris/detail/type_traits.hpp>

namespace iris {

template <typename T, template <typename...> class U>
struct is_specialized_of : std::false_type {
};

template <template <typename...> class U, typename... Args>
struct is_specialized_of<U<Args...>, U> : std::true_type {
};

template <typename T, template <typename...> class U>
inline constexpr bool is_specialized_of_v = is_specialized_of<T, U>::value;

template <typename E, bool = std::is_enum_v<E>>
struct is_scoped_enum : std::false_type {
};

template <typename E>
struct is_scoped_enum<E, true>
    : std::bool_constant<!std::is_convertible_v<E, std::underlying_type_t<E>>> {
};

template <typename E>
inline constexpr bool is_scoped_enum_v = is_scoped_enum<E>::value;

}
