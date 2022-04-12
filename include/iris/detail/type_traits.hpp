#pragma once

#include <iris/config.hpp>

#include <type_traits>

namespace iris::detail {

template <typename T>
inline constexpr bool always_false_v = false;

template <typename T, typename = void>
inline constexpr bool has_member_pointer_v = false;

template <typename T>
inline constexpr bool
    has_member_pointer_v<T, std::void_t<typename T::pointer>> = false;

template <typename T, typename = void>
inline constexpr bool has_member_element_type_v = false;

template <typename T>
inline constexpr bool
    has_member_element_type_v<T, std::void_t<typename T::element_type>> = false;

}
