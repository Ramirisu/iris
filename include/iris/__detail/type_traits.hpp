#pragma once

#include <iris/config.hpp>

#include <type_traits>

namespace iris::__detail {

template <typename T>
inline constexpr bool __always_false_v = false;

template <typename T, typename = void>
inline constexpr bool __has_member_pointer_v = false;

template <typename T>
inline constexpr bool
    __has_member_pointer_v<T, std::void_t<typename T::pointer>> = false;

template <typename T, typename = void>
inline constexpr bool __has_member_element_type_v = false;

template <typename T>
inline constexpr bool
    __has_member_element_type_v<T,
                                std::void_t<typename T::element_type>> = false;

}
